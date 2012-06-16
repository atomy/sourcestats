#include "CGameQuery.h"
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#define CNAME "CGameQuery"

CGameQuery::CGameQuery(const char* strHostname, const char* strPort)
{
	// TODO, wtf is this?
	m_strQueryTargetPort = strPort;
	m_strQueryTargetHostname = strHostname;
	m_targetServer.setIp(strHostname);
	m_targetServer.setPort(strPort);
}

CGameQuery::~CGameQuery()
{
	freeaddrinfo(m_pServerinfo);
	freeaddrinfo(m_pSelectedServ);
	close(m_iSocket);

	m_iSocket = -1;
}

bool CGameQuery::init()
{
	struct addrinfo hints;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;

	if((getaddrinfo(m_strQueryTargetHostname, m_strQueryTargetPort, &hints, &m_pServerinfo)) != 0) {
		g_pLogger->AddLog(CNAME, __FUNCTION__, "error while getaddrinfo()");
		return false;
	}

	for(m_pSelectedServ = m_pServerinfo; m_pSelectedServ != NULL; m_pSelectedServ = m_pSelectedServ->ai_next) {
		if ((m_iSocket = socket(m_pSelectedServ->ai_family, m_pSelectedServ->ai_socktype, m_pSelectedServ->ai_protocol)) == -1) {
			g_pLogger->AddLog(CNAME, __FUNCTION__, "socket()");
			continue;
		}
		break;
	}

	if(m_pSelectedServ == NULL) {
		g_pLogger->AddLog(CNAME, __FUNCTION__, "failed to bind socket()");
		return false;
	}

	return true;
}

void CGameQuery::doRequest()
{	
	g_pLogger->AddLog(CNAME, __FUNCTION__, "lets call retrieveServerInfo()");
	retrieveServerInfo();
}

void CGameQuery::retrieveServerInfo()
{
	memset(m_strRecvBuffer, 0, RECVBUF_SIZE);
	memset(m_strSendBuffer, 0, SENDBUFFER_SIZE);

	char log[128];

	// receive header, FF FF FF FF 66 0A
	// byte_ip1, byte_ip2, byte_ip3, byte_ip4, unsigned short (network ordered)

	// fill send buffer and set the real size we are going to need
	int iSendLen = buildSendBuffer(m_strSendBuffer, SENDBUFFER_SIZE);
	if(iSendLen > SENDBUFFER_SIZE)
		iSendLen = SENDBUFFER_SIZE;

	if((m_iSentBytes = sendto(m_iSocket, m_strSendBuffer, iSendLen, 0, m_pSelectedServ->ai_addr, m_pSelectedServ->ai_addrlen)) == -1) {
		g_pLogger->AddLog(CNAME, __FUNCTION__, "error while sendto()");
	}

	snprintf(log, 128, "sent '%d' bytes to '%s:%s'", m_iSentBytes, m_strQueryTargetHostname, m_strQueryTargetPort);
	g_pLogger->AddLog(CNAME, __FUNCTION__, log);

	if((m_iRecvBytes = recvfrom(m_iSocket, m_strRecvBuffer, RECVBUF_SIZE, 0, NULL, NULL)) == -1) {
		snprintf(log, 128, "error while recvfrom() code: '%s'", strerror(errno));
		g_pLogger->AddLog(CNAME, __FUNCTION__, log);
	}

	snprintf(log, 128, "received '%d' bytes", m_iRecvBytes);
	g_pLogger->AddLog(CNAME, __FUNCTION__, log);

	parseData();
}


int CGameQuery::buildSendBuffer(unsigned char* buf, size_t size) {
	memset(buf, 0, size);

	char serv[64];
	m_targetServer.toString(serv, 64);

	char log[128];
	snprintf(log, 128, "building send buffer, using gameserver: '%s'", serv);
	g_pLogger->AddLog(CNAME, __FUNCTION__, log);

	// AS_INFO query [http://developer.valvesoftware.com/wiki/Server_Queries]
	// FF FF FF FF 54 53 6F 75 72 63 65 20 45 6E 67 69   ÿÿÿÿTSource Engi
	// 6E 65 20 51 75 65 72 79 00                        ne Query
	// "\xFF" "\xFF" "\xFF" "\xFF" "TSource Engine Query" "\x00"

	const char* init = "TSource Engine Query";
	const int iInitStart = 4;
	const int iInitEnd = strlen(init)+iInitStart;

	for(int i = 0; i < (int)size; i++) {
		if(i < 4) {
			buf[i] = '\xFF';
		}	else if(i >= iInitStart && i <= iInitEnd) {
			buf[i] = init[i-iInitStart];
		} else if(i == iInitEnd+1) {
			buf[i] = '\x00';
		}
	}
	return iInitEnd+1+1;
}

// parse our buffer and return last server
void CGameQuery::parseData()
{
	char log[128];
	size_t read = 4*sizeof(unsigned char);

	snprintf(log, 128, "got '%d' bytes from \'???\'", m_iRecvBytes);
	g_pLogger->AddLog(CNAME, __FUNCTION__, log);

	m_infoResult.m_addr = m_targetServer;

	char strBuf[256];

	// type, byte
	memcpy(&m_infoResult.type, m_strRecvBuffer+read, sizeof(m_infoResult.type));
	read += sizeof(m_infoResult.type);

	// version, byte
	memcpy(&m_infoResult.version, m_strRecvBuffer+read, sizeof(m_infoResult.version));
	read += sizeof(m_infoResult.version);

	// server name, string
	strncpy(strBuf, (char*)m_strRecvBuffer+read, sizeof(strBuf));
	read += strlen(strBuf)+1;
	m_infoResult.setServerName(strBuf);

	// map, string
	strncpy(strBuf, (char*)m_strRecvBuffer+read, sizeof(strBuf));
	read += strlen(strBuf)+1;
	m_infoResult.setMap(strBuf);

	// game directory, string
	strncpy(strBuf, (char*)m_strRecvBuffer+read, sizeof(strBuf));
	read += strlen(strBuf)+1;
	m_infoResult.setGameDir(strBuf);

	// game description, string
	strncpy(strBuf, (char*)m_strRecvBuffer+read, sizeof(strBuf));
	read += strlen(strBuf)+1;
	m_infoResult.setGameDesc(strBuf);

	// appid, short
	memcpy(&m_infoResult.appid, m_strRecvBuffer+read, sizeof(m_infoResult.appid));
	read += sizeof(m_infoResult.appid);

	// number of players, byte
	memcpy(&m_infoResult.playercount, m_strRecvBuffer+read, sizeof(m_infoResult.playercount));
	read += sizeof(m_infoResult.playercount);

	// maximum players, byte
	memcpy(&m_infoResult.maxplayers, m_strRecvBuffer+read, sizeof(m_infoResult.maxplayers));
	read += sizeof(m_infoResult.maxplayers);

	// number of bots, byte
	memcpy(&m_infoResult.bots, m_strRecvBuffer+read, sizeof(m_infoResult.bots));
	read += sizeof(m_infoResult.bots);

	// dedicated, byte
	memcpy(&m_infoResult.dedicated, m_strRecvBuffer+read, sizeof(m_infoResult.dedicated));
	read += sizeof(m_infoResult.dedicated);

	// os, byte
	memcpy(&m_infoResult.os, m_strRecvBuffer+read, sizeof(m_infoResult.os));
	read += sizeof(m_infoResult.os);

	// password, byte
	memcpy(&m_infoResult.password, m_strRecvBuffer+read, sizeof(m_infoResult.password));
	read += sizeof(m_infoResult.password);

	// secure, byte
	memcpy(&m_infoResult.secure, m_strRecvBuffer+read, sizeof(m_infoResult.secure));
	read += sizeof(m_infoResult.secure);

	// game version, string
	strncpy(strBuf, (char*)m_strRecvBuffer+read, sizeof(strBuf));
	read += strlen(strBuf)+1;
	m_infoResult.setGameVersion(strBuf);

	// extra data flag, byte
	// servers game port, short
	// servers steam id, long long
	// spectator port + spectator server name, short + string
	// game tag data, string
	// servers 64bit gameid, long long

	//snprintf(log, 128, "servername: '%s'", m_infoResult.getServerName());
	//g_pLogger->AddLog(CNAME, __FUNCTION__, log);

	//snprintf(log, 128, "map: '%s'", m_infoResult.getMapName());
	//g_pLogger->AddLog(CNAME, __FUNCTION__, log);

	//snprintf(log, 128, "gameDir: '%s'", m_infoResult.getGameDir());
	//g_pLogger->AddLog(CNAME, __FUNCTION__, log);

	//snprintf(log, 128, "gameVersion: '%s'", m_infoResult.getGameVersion());
	//g_pLogger->AddLog(CNAME, __FUNCTION__, log);

	//snprintf(log, 128, "gameDesc: '%s'", m_infoResult.getGameDesc());
	//g_pLogger->AddLog(CNAME, __FUNCTION__, log);
}

CInfoStruct& CGameQuery::getInfoStruct()
{
	return m_infoResult;
}
