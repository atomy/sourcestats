#include "const.h"
#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <stdio.h>
#include <iostream>
#include "Masterquery.h"

#define FINGERPRINT_SIZE 6
#define MQ_MAXTRIES 2

using boost::asio::ip::udp;
extern pthread_mutex_t muLog;

Masterquery::Masterquery( ThreadFactory* pFactory ) : ThreadedRequest( pFactory )
{
    m_iState = MQSTATE_NEW;
	//SetParentClassName( "Masterquery" );
}

Masterquery::~Masterquery()
{
}

void Masterquery::SetMaster( servAddr mAddr )
{
    char strIp[16];
    char strPort[8];
	char logout[128];

    servAddr2Ip( strIp, 16, mAddr );
    servAddr2Port( strPort, 8, mAddr );
	snprintf(logout, 128, "Masterquery::SetMaster() setting master server to '%s:%s'", strIp, strPort);
    Log(logout);
    masterAddr = mAddr;
}

void Masterquery::SetGame( const char* gName )
{
    strncpy( gameName, gName, sizeof(gameName) );
}

void Masterquery::Exec( void )
{
    char strIp[16];
    char strPort[8];
	char logout[128];

    servAddr2Ip( strIp, 16, masterAddr );
    servAddr2Port( strPort, 8, masterAddr );
	snprintf(logout, 128, "Masterquery::Exec() requesting gameserver for game '%s' -- using master server '%s:%s'", gameName, strIp, strPort);
    Log(logout);
    Query();
}

void Masterquery::EntryPoint( void )
{
	ThreadedRequest::PreEntryPoint();
	Exec();
	ThreadedRequest::PostEntryPoint();
}

servAddr Masterquery::ParseMasterReply(const char* recvData, size_t len)
{
    size_t read = 0;
    servAddr nullAddr;
    nullAddr.ip1 = 0;
    nullAddr.ip2 = 0;
    nullAddr.ip3 = 0;
    nullAddr.ip4 = 0;
    nullAddr.port = 0;

    // checking fingerprint
    unsigned char finger[FINGERPRINT_SIZE];
    memcpy(finger, recvData, sizeof(finger));
    read += sizeof(finger);

    // FF FF FF FF 66 0A
    if (finger[0] != 0xFF || finger[1] != 0xFF || finger[2] != 0xFF || finger[3] != 0xFF || finger[4] != 0x66 || finger[5] != 0xA)
    {
        char sServAddr[128];
        servAddr2String(sServAddr, 128, masterAddr );
	#ifdef DEBUG
	//std::cout << "[" << time(NULL) << "][THREAD|" << GetThreadId() << "]
        fprintf(stderr, "[%d][THREAD|%d] Masterquery::ParseMasterReply() invalid fingerprint received while querying: %s for game %s\n", (int)time(NULL), (int)GetThreadId(), sServAddr, gameName);
	#endif

        return nullAddr;
    }

    servAddr lastGameAddr;
    size_t entryLen = 0;

    while ( read < len )
    {
        servAddr gIp;
        //char output[128];
        unsigned short htPort;
        size_t readStart = read;

        memcpy(&gIp.ip1, recvData+read, sizeof(gIp.ip1));
        read += sizeof(gIp.ip1);
        memcpy(&gIp.ip2, recvData+read, sizeof(gIp.ip2));
        read += sizeof(gIp.ip2);
        memcpy(&gIp.ip3, recvData+read, sizeof(gIp.ip3));
        read += sizeof(gIp.ip3);
        memcpy(&gIp.ip4, recvData+read, sizeof(gIp.ip4));
        read += sizeof(gIp.ip4);
        memcpy(&htPort, recvData+read, sizeof(htPort));
        read += sizeof(htPort);
        gIp.port = ntohs(htPort);

        //servAddr2String( output, 128, gIp );
        //printf( "%s\n", output );

        lastGameAddr = gIp;

        // check for end of list, the master server returns 0.0.0.0:0 as address on EOF
        if ( lastGameAddr.ip1 == 0 && lastGameAddr.ip2 == 0 && lastGameAddr.ip3 == 0 && lastGameAddr.ip4 == 0 || lastGameAddr.port == 0 )
        {
            Log("Masterquery::ParseMasterReply() EOF received, giving up!");
            return lastGameAddr;
        }
        entryLen = read - readStart;

        // add gameserver to the list for further usage
        AddEntry(new GameserverEntry(lastGameAddr));
    }
    return lastGameAddr;
}

servAddr Masterquery::RequestMore( udp::socket* socket, servAddr gIp )
{
    char output[128];
    char sQuery[32];
    char ip[16];
    char port[8];
	char logout[128];

    servAddr2Ip( ip, 16, masterAddr );
    servAddr2Port( port, 8, masterAddr );
    servAddr2String( output, 128, gIp );

	snprintf(logout, 128, "Masterquery::RequestMore() using seed %s", output);
	Log(logout);
    char queryString[256];
    snprintf(sQuery, 256, "1" "\xFF" "%u.%u.%u.%u:%u" "\x00" "\\gamedir\\%s\\napp\\500" "\x00", gIp.ip1, gIp.ip2, gIp.ip3, gIp.ip4, gIp.port, gameName);
	snprintf(logout, 128, "Masterquery::RequestMore() querying '%s:%s' with string: '%s'", ip, port, queryString);
	Log(logout);

    boost::asio::io_service io_service;

    udp::resolver resolver(io_service);
    udp::resolver::query query(udp::v4(), ip, port );
    udp::endpoint receiver_endpoint = *resolver.resolve(query);

    // send
    socket->send_to(boost::asio::buffer(sQuery), receiver_endpoint);

    boost::array<char, 5120> recv_buf;
    udp::endpoint sender_endpoint;

	Log("Masterquery::RequestMore() waiting for reply(2)...");
    // wait for reply
    size_t len = socket->receive_from(
                     boost::asio::buffer(recv_buf), sender_endpoint);

    return ParseMasterReply( recv_buf.data(), len );
}


void Masterquery::Query( void )
{
    try
    {
        char ip[16];
        char port[8];

        servAddr2Ip( ip, 16, masterAddr );
        servAddr2Port( port, 8, masterAddr );
        boost::asio::io_service io_service;

        udp::resolver resolver(io_service);
        udp::resolver::query query(udp::v4(), ip, port);
        udp::endpoint receiver_endpoint = *resolver.resolve(query);

        udp::socket socket(io_service);
        socket.open(udp::v4());

        // send 1:0.0.0.0:0 to retrieve all servers
        char queryString[256];
		char logout[128];
        snprintf( queryString, 256, "1%c0.0.0.0:0%c\\gamedir\\%s\\napp\\500%c", 255, 0 ,gameName, 0 );
		snprintf(logout, 128, "Masterquery::Query() querying '%s:%s' with string: '%s'", ip, port, queryString);
		Log(logout);
        socket.send_to(boost::asio::buffer(queryString), receiver_endpoint);

        boost::array<char, 5120> recv_buf;
        udp::endpoint sender_endpoint;
		Log("Masterquery::Query() waiting for reply...");

        // wait for reply
        size_t len = socket.receive_from(
                         boost::asio::buffer(recv_buf), sender_endpoint);

        servAddr gIp;
        gIp = ParseMasterReply(recv_buf.data(), len);
		Log("Masterquery::Query() parsed reply!");

        int loops = 0;
        while ( gIp.ip1 != 0 || gIp.ip2 != 0 || gIp.ip3 != 0 || gIp.ip4 != 0 || gIp.port != 0 )
        {
            if ( loops >= 500 )
            {
				Log("Masterquery::Query() hard break");
                break;
            }

            loops++;
			Log("Masterquery::Query() requesting more...");
            gIp = RequestMore(&socket, gIp);
        }
		Log("Masterquery::Query() EOF!");
		Finished();
    }
    catch (std::exception& e)
    {
		std::cerr << "[" << time(NULL) << "][THREAD|" << GetThreadId() << "] Masterquery::Query() exception raised: " << e.what() << std::endl;
    }
}

// we are done getting servers, let dad know!
void Masterquery::Finished( void )
{
    m_iState = MQSTATE_DONE;
}

void Masterquery::AddEntry( GameserverEntry* pEntry )
{
    char output[128];
	char logout[128];
    servAddr2String( output, 128, pEntry->GetAddr() );
	snprintf(logout, 128, "Masterquery::AddEntry() added new entry with address: '%s'", output);
    Log(logout);

    // lock is handled by state, we arent accessing this element before its complete,
    // could use some improvement though we rely on calling funcs that they obey that rule
    m_vResultlist.push_back( pEntry );
}

void Masterquery::ResetIterator( void )
{
	Log("Masterquery::ResetIterator()");
    m_geIT = m_vResultlist.begin();
}

GameserverEntry* Masterquery::GetNextServer( void )
{
    if ( m_vResultlist.size() <= 0 )
		Log("Masterquery::GetNextServer() error, list is empty!");

	if ( m_geIT == m_vResultlist.end() )
        return NULL;

	GameserverEntry* gEntry = (*m_geIT);
	m_geIT++;

    return gEntry;
}

void Masterquery::Log( const char* logMsg )
{
    pthread_mutex_lock (&muLog);
	std::cout << "[" << time(NULL) << "][THREAD:" << GetThreadId() << "|G:" << gameName << "] "<< logMsg << std::endl;
    pthread_mutex_unlock (&muLog);
}
