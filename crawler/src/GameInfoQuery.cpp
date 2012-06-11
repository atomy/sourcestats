#include "GameInfoQuery.h"
#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <iostream>
#include "stdio.h"
#include <string>
#include "const.h"
#include "GameStats.h"
#include "GameInfoQuery.h"

using boost::asio::ip::udp;
extern pthread_mutex_t muLog;

GameInfoQuery::GameInfoQuery( ThreadFactory* pFactory, servAddr sAddr ) : ThreadedRequest( pFactory )
{
    m_iState = GISTATE_NEW;
	//SetParentClassName( "GameInfoQuery" );
	m_pGSInfo = new GameserverInfo(sAddr);
}

void GameInfoQuery::EntryPoint( void )
{
	ThreadedRequest::PreEntryPoint();
	QueryforASINFO();
	ThreadedRequest::PostEntryPoint();
}

void GameInfoQuery::QueryforASINFO( void )
{
    servAddr sAddr = m_pGSInfo->GetAddr();
    try
    {
        boost::asio::io_service io_service;
        char targetAddr[32];
        char targetPort[8];
        snprintf(targetAddr, sizeof(targetAddr), "%d.%d.%d.%d\0", sAddr.ip1, sAddr.ip2, sAddr.ip3, sAddr.ip4);
        snprintf(targetPort, sizeof(targetPort), "%d\0", sAddr.port);

        udp::resolver resolver(io_service);
        udp::resolver::query query(udp::v4(), targetAddr, targetPort);
        udp::endpoint receiver_endpoint = *resolver.resolve(query);

        udp::socket socket(io_service);
        socket.open(udp::v4());

        // AS_INFO query [http://developer.valvesoftware.com/wiki/Server_Queries]
        // FF FF FF FF 54 53 6F 75 72 63 65 20 45 6E 67 69   ÿÿÿÿTSource Engi
        // 6E 65 20 51 75 65 72 79 00                        ne Query
        socket.send_to(boost::asio::buffer("\xFF" "\xFF" "\xFF" "\xFF" "TSource Engine Query" "\x00"), receiver_endpoint);

		char logtxt[128];
		snprintf( logtxt, 128, "Masterquery::QueryforASINFO() sent AS_INFO to %s:%s", targetAddr, targetPort );
        Log( logtxt );

        boost::array<char, 5120> recv_buf;
        udp::endpoint sender_endpoint;

		Log( "Masterquery::QueryforASINFO() waiting for reply..." );
        m_iState = GISTATE_WAITINGFORREPLY;

        // wait for reply
        size_t len = socket.receive_from(
                         boost::asio::buffer(recv_buf), sender_endpoint);

        ParseASINFO(recv_buf.data(), len);
    }
    catch (std::exception& e)
    {
        std::cerr << "exception raised: " << e.what() << std::endl;
    }
}

void GameInfoQuery::ParseASINFO(const char* recvData, size_t len)
{
    size_t read = 0;

    // checking fingerprint
    unsigned char finger[4];
    memcpy(finger, recvData, sizeof(finger));
    read += sizeof(finger);

    // FF FF FF FF
    if (finger[0] != 0xFF || finger[1] != 0xFF || finger[2] != 0xFF || finger[3] != 0xFF)
    {
        fprintf(stderr, "GameInfoRequest::ParseASINFO() invalid fingerprint received\n");
        return;
    }

    unsigned char ucBuf;
    unsigned short usBuf;
    char strBuf[256];

    // Type byte
    memcpy(&ucBuf,recvData+read,sizeof(ucBuf));
    read += sizeof(ucBuf);
    m_pGSInfo->SetType(ucBuf);

    // Version byte
    memcpy(&ucBuf,recvData+read,sizeof(ucBuf));
    read += sizeof(ucBuf);
    m_pGSInfo->SetVersion(ucBuf);

    strncpy(strBuf,recvData+read,sizeof(strBuf));
    read += strlen(strBuf)+1;
    m_pGSInfo->SetServerName(strBuf);

    strncpy(strBuf,recvData+read,sizeof(strBuf));
    read += strlen(strBuf)+1;
    m_pGSInfo->SetCurrentMap(strBuf);

    strncpy(strBuf,recvData+read,sizeof(strBuf));
    read += strlen(strBuf)+1;
    m_pGSInfo->SetGameDir(strBuf);

    strncpy(strBuf,recvData+read,sizeof(strBuf));
    read += strlen(strBuf)+1;
    m_pGSInfo->SetGameDesc(strBuf);

    memcpy(&usBuf,recvData+read,sizeof(usBuf));
    read += sizeof(usBuf);
    m_pGSInfo->SetAppid(usBuf);

    memcpy(&ucBuf,recvData+read,sizeof(ucBuf));
    read += sizeof(ucBuf);
    m_pGSInfo->SetPlayerCount(ucBuf);

    memcpy(&ucBuf,recvData+read,sizeof(ucBuf));
    read += sizeof(ucBuf);
    m_pGSInfo->SetMaxPlayers(ucBuf);

    memcpy(&ucBuf,recvData+read,sizeof(ucBuf));
    read += sizeof(ucBuf);
    m_pGSInfo->SetBotCount(ucBuf);

    memcpy(&ucBuf,recvData+read,sizeof(ucBuf));
    read += sizeof(ucBuf);
    m_pGSInfo->SetDedicated(ucBuf);

    memcpy(&ucBuf,recvData+read,sizeof(ucBuf));
    read += sizeof(ucBuf);
    m_pGSInfo->SetOS(ucBuf);

    memcpy(&ucBuf,recvData+read,sizeof(ucBuf));
    read += sizeof(ucBuf);
    m_pGSInfo->SetPassworded(ucBuf);

    memcpy(&ucBuf,recvData+read,sizeof(ucBuf));
    read += sizeof(ucBuf);
    m_pGSInfo->SetSecure(ucBuf);

    strncpy(strBuf,recvData+read,sizeof(strBuf));
    read += strlen(strBuf)+1;
    m_pGSInfo->SetGameversion(strBuf);

    Log("Masterquery::ParseASINFO() got reply!");
//printf( "GameInfoRequest::ParseASINFO() - got reply!\n" );
/*\
      type: %x \
      version: %x \
      servername: %s \
      gamedir: %s \
      gamedesc: %s \
      appid: %d \
      playercount: %d \
      maxplayers: %d \
      bots: %d \
      dedicated: %c \
      os: %c \
      pass: %x \
      secure: %x \
      gameversion: %s \
      \n", m_pGSInfo->m_cType, m_pGSInfo->m_cVersion, m_pGSInfo->m_sServername.c_str(),
	     m_pGSInfo->m_sGamedir.c_str(), m_pGSInfo->m_sGamedesc.c_str(),
	     m_pGSInfo->m_iAppid, m_pGSInfo->m_cPlayercount, m_pGSInfo->m_cMaxplayers, m_pGSInfo->m_cBotcount,
	     m_pGSInfo->m_cDedicated, m_pGSInfo->m_cOS, m_pGSInfo->m_cISPassworded, m_pGSInfo->m_cISSecure,
	     m_pGSInfo->m_sGameversion.c_str() );
*/
    m_iState = GISTATE_DONE;
	//m_pParent->GameInfoDoneCallback( this );
}

// returns result if its done yet, if not, false
//GameserverInfo* GameInfoQuery::GetResult( void )
//{
//    if ( m_iState == STATE_DONE )
//        return m_pGSInfo;
//    else
//        return false;
//}

void GameInfoQuery::Log( const char* logMsg )
{
    pthread_mutex_lock (&muLog);
	std::cout << "[" << time(NULL) << "][THREAD|" << GetThreadId() << "] " << logMsg << std::endl;
    pthread_mutex_unlock (&muLog);
}