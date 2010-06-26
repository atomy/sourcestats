#include "const.h"
#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <stdio.h>
#include <iostream>
#include "Masterquery.h"

#define FINGERPRINT_SIZE 6

using boost::asio::ip::udp;


Masterquery::Masterquery()
{
    m_iState = STATE_NEW;
}

void Masterquery::SetMaster( servAddr mAddr )
{
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

    servAddr2Ip( strIp, 16, masterAddr );
    servAddr2Port( strPort, 8, masterAddr );
#ifdef DEBUG
    std::cout << "Masterquery::Exec() requesting gameserver for game '" << gameName << "' -- using master server '" << strIp << ":" << strPort << "'" << endl;
#endif
    Query();
}

servAddr Masterquery::ParseMasterReply(const char* recvData, size_t len)
{
#ifdef DEBUG
    std::cout << "Masterquery::ParseMasterReply() len: " << len << std::endl;
#endif
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
        fprintf(stderr, "Masterquery::ParseMasterReply() invalid fingerprint received while querying: %s for game %s\n", sServAddr, gameName);
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
            printf("Masterquery::ParseMasterReply() EOF received, giving up!\n");
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

    servAddr2Ip( ip, 16, masterAddr );
    servAddr2Port( port, 8, masterAddr );
    servAddr2String( output, 128, gIp );

#ifdef DEBUG
    std::cout << "Masterquery::RequestMore() using seed " << output << std::endl;
#endif
    char queryString[256];
    snprintf(sQuery, 256, "1" "\xFF" "%u.%u.%u.%u:%u" "\x00" "\\gamedir\\%s\\napp\\500" "\x00", gIp.ip1, gIp.ip2, gIp.ip3, gIp.ip4, gIp.port, gameName);
#ifdef DEBUG
    std::cout << "Masterquery::RequestMore() querying " << ip << ":" << port << "with string: " << queryString << endl;
#endif
    boost::asio::io_service io_service;

    udp::resolver resolver(io_service);
    udp::resolver::query query(udp::v4(), ip, port );
    udp::endpoint receiver_endpoint = *resolver.resolve(query);

    // send
    socket->send_to(boost::asio::buffer(sQuery), receiver_endpoint);

    boost::array<char, 5120> recv_buf;
    udp::endpoint sender_endpoint;
#ifdef DEBUG
    std::cout << "Masterquery::RequestMore() waiting for reply(2)..." << std::endl;
#endif
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
        snprintf( queryString, 256, "1%c0.0.0.0:0%c\\gamedir\\%s\\napp\\500%c", 255, 0 ,gameName, 0 );
#ifdef DEBUG
        std::cout << "Masterquery::Query() querying " << ip << ":" << port << " with string: " << queryString << endl;
#endif
        socket.send_to(boost::asio::buffer(queryString), receiver_endpoint);

        boost::array<char, 5120> recv_buf;
        udp::endpoint sender_endpoint;
#ifdef DEBUG
        std::cout << "Masterquery::Query() waiting for reply..." << std::endl;
#endif
        // wait for reply
        size_t len = socket.receive_from(
                         boost::asio::buffer(recv_buf), sender_endpoint);

        servAddr gIp;
        gIp = ParseMasterReply(recv_buf.data(), len);
#ifdef DEBUG
        std::cout << "Masterquery::Query() parsed reply!" << std::endl;
#endif
        int loops = 0;
        while ( gIp.ip1 != 0 || gIp.ip2 != 0 || gIp.ip3 != 0 || gIp.ip4 != 0 || gIp.port != 0 )
        {
            if ( loops >= 20 )
            {
#ifdef DEBUG
                std::cerr << "Masterquery::Query() hard break" << std::endl;
#endif
                break;
            }

            loops++;
#ifdef DEBUG
            std::cout << "Masterquery::Query() requesting more..." << std::endl;
#endif
            exit(0);
            gIp = RequestMore(&socket, gIp);
        }
#ifdef DEBUG
        std::cout << "Masterquery::Query() EOF!" << std::endl;
#endif
        m_iState = STATE_DONE;
		Finished();
    }
    catch (std::exception& e)
    {
#ifdef DEBUG
        std::cerr << "Masterquery::Query() exception raised: " << e.what() << std::endl;
#endif
    }
}

void Masterquery::Finished( void )
{
	m_pParent->MasterqueryDoneCallback();
}

void Masterquery::AddEntry( GameserverEntry* pEntry )
{
    char output[128];
    servAddr2String( output, 128, pEntry->GetAddr() );
#ifdef DEBUG
    std::cout << "Masterquery::AddEntry() added new entry with address: " << output << endl;
#endif

    m_vResultlist.push_back( pEntry );
}

void Masterquery::ResetIterator( void )
{
    m_geIT = m_vResultlist.begin();
}

GameserverEntry* Masterquery::GetNextServer( void )
{
    if ( m_vResultlist.size() <= 0 )
    {
#ifdef DEBUG
        std::cerr << "Masterquery::GetNextServer() error, list is empty!" << endl;
#endif
    }

    if ( m_geIT == m_vResultlist.end() )
        return NULL;

    GameserverEntry* gEntry = (*m_geIT);
    m_geIT++;

    return gEntry;
}
