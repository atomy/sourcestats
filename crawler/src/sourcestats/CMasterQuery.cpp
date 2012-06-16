#include "CMasterQuery.h"
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#define CNAME "CMasterQuery"

CMasterQuery::CMasterQuery(const char* strHostname, const char* strPort)
{
	// TODO, overwrite assign operator!
	//m_cMasterServ = strMasterServer;
	m_strMasterServerHostname = strHostname;
	m_strMasterServerPort = strPort;
}

CMasterQuery::~CMasterQuery()
{
	while(!m_pServers.empty()) {
		CServAddr* pServ = m_pServers.top();
		m_pServers.pop();
		delete pServ;
	}

	freeaddrinfo(m_pServerinfo);
	freeaddrinfo(m_pSelectedServ);
	close(m_iSocket);

	m_iSocket = -1;
}

bool CMasterQuery::init()
{
	struct addrinfo hints;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;

	if((getaddrinfo(m_strMasterServerHostname, m_strMasterServerPort, &hints, &m_pServerinfo)) != 0) {
		char log[128];
		snprintf(log, 128, "error while getaddrinfo() host: '%s' port: '%s'", m_strMasterServerHostname, m_strMasterServerPort);
		g_pLogger->AddLog(CNAME, __FUNCTION__, log);
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

void CMasterQuery::doRequest()
{	
	char log[128];
	snprintf(log, 128, "lets call retrieveServers() with '%s'", "0.0.0.0:0");
	g_pLogger->AddLog(CNAME, __FUNCTION__, log);

	char addr[64] = {0};
	for(CServAddr cServAddr = retrieveServers(CServAddr("0.0.0.0:0")); cServAddr.ip1 != 0; cServAddr = retrieveServers(addr))  {
		cServAddr.toString(addr, 64);
	}
}

CServAddr CMasterQuery::retrieveServers(CServAddr serverSeed)
{
	memset(m_strRecvBuffer, 0, RECVBUF_SIZE);
	memset(m_strSendBuffer, 0, SENDBUFFER_SIZE);

	char log[128];

	// receive header, FF FF FF FF 66 0A
	// byte_ip1, byte_ip2, byte_ip3, byte_ip4, unsigned short (network ordered)

	// fill send buffer and set the real size we are going to need
	int iSendLen = buildSendBuffer(serverSeed, m_strSendBuffer, SENDBUFFER_SIZE);
	if(iSendLen > SENDBUFFER_SIZE)
		iSendLen = SENDBUFFER_SIZE;

	if((m_iSentBytes = sendto(m_iSocket, m_strSendBuffer, iSendLen, 0, m_pSelectedServ->ai_addr, m_pSelectedServ->ai_addrlen)) == -1) {
		g_pLogger->AddLog(CNAME, __FUNCTION__, "error while sendto()");
	}

	snprintf(log, 128, "sent '%d' bytes to '%s'", m_iSentBytes, m_strMasterServerHostname);
	g_pLogger->AddLog(CNAME, __FUNCTION__, log);

	if((m_iRecvBytes = recvfrom(m_iSocket, m_strRecvBuffer, RECVBUF_SIZE, 0, NULL, NULL)) == -1) {
		snprintf(log, 128, "error while recvfrom() code: '%s'", strerror(errno));
		g_pLogger->AddLog(CNAME, __FUNCTION__, log);
	}

	return parseData();
}


int CMasterQuery::buildSendBuffer(CServAddr& addr, unsigned char* buf, size_t size) {
	memset(buf, 0, size);

	char seed[64];
	addr.toString(seed, 64);

	char log[128];
	snprintf(log, 128, "building send buffer, using seed: '%s'", seed);
	g_pLogger->AddLog(CNAME, __FUNCTION__, log);

	//const char* seed = "0.0.0.0:0";
	const char* filter = "\\gamedir\\Dystopia\\napp\\500";
	const int iSeedStart = 2;
	const int iSeedEnd = strlen(seed)+iSeedStart-1;
	const int iFilterStart = iSeedEnd+1;
	const int iFilterEnd = iFilterStart+strlen(filter);

	for(int i = 0; i < (int)size; i++) {
		if(i == 0) {
			buf[i] = '\x31';
		}	else if(i == 1) {
			buf[i] = '\xFF';
		} else if(iSeedStart <= i && i <= iSeedEnd) {
			buf[i] = seed[i-iSeedStart];
		} else if(i == iSeedEnd+1) {
			buf[i] = '\x00';
		} else if(iFilterStart <= i && i <= iFilterEnd) {
			buf[i] = filter[i-iFilterStart-1];
		} else if(i == iFilterEnd+1) {
			buf[i] = '\x00';
		}
	}	
	return iFilterEnd+1+1;
}

// parse our buffer and return last server
CServAddr CMasterQuery::parseData()
{
	char log[128];
	size_t read = 6;
	CServAddr lastaddr;

	snprintf(log, 128, "got '%d' bytes from \'???\'", m_iRecvBytes);
	g_pLogger->AddLog(CNAME, __FUNCTION__, log);

	//snprintf(log, 128, "received: '%s'", m_strRecvBuffer);
	//g_pLogger->AddLog(CNAME, __FUNCTION__, log);

	while((int)read < m_iRecvBytes) {
		unsigned short netport;

		memcpy(&lastaddr.ip1, m_strRecvBuffer+read, sizeof(lastaddr.ip1));
		read += sizeof(lastaddr.ip1);
		memcpy(&lastaddr.ip2, m_strRecvBuffer+read, sizeof(lastaddr.ip2));
		read += sizeof(lastaddr.ip2);
		memcpy(&lastaddr.ip3, m_strRecvBuffer+read, sizeof(lastaddr.ip3));
		read += sizeof(lastaddr.ip3);
		memcpy(&lastaddr.ip4, m_strRecvBuffer+read, sizeof(lastaddr.ip4));
		read += sizeof(lastaddr.ip4);
		memcpy(&netport, m_strRecvBuffer+read, sizeof(netport));
		read += sizeof(unsigned short);
		lastaddr.port = ntohs(netport);

		char buf[64];
		lastaddr.toString(buf, 64);
		snprintf(log, 128, "server: '%s'", buf);
		g_pLogger->AddLog(CNAME, __FUNCTION__, log);

		m_pServers.push(new CServAddr(lastaddr));
	}

	m_pServers.pop();

	snprintf(log, 128, "parseData() completed we have '%d' servers now in the list", m_pServers.size());
	g_pLogger->AddLog(CNAME, __FUNCTION__, log);

	return lastaddr;
}