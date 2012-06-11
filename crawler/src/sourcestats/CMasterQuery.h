#ifndef CMASTER_QUERY_H
#define CMASTER_QUERY_H

#include "CServAddr.h"
#include <vector>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include "libcursesfrontend/IDisplayLogger.h"
#include <stack>

#define RECVBUF_SIZE 8192
#define SENDBUFFER_SIZE 128

using namespace std;

class CMasterQuery
{
public:
	CMasterQuery(const char* strHostname, const char* strPort);
	~CMasterQuery();

	bool											init();

	void											doRequest();
	int												buildSendBuffer(CServAddr& addr, unsigned char* buf, size_t size);
	CServAddr									retrieveServers(CServAddr serverSeed);

private:
	CServAddr									parseData();

	const char*								m_strMasterServerPort;
	const char*								m_strMasterServerHostname;
	int												m_iSocket;
	int												m_iRecvBytes, m_iSentBytes;
	struct addrinfo						*m_pServerinfo, *m_pSelectedServ;
	unsigned char							m_strSendBuffer[SENDBUFFER_SIZE];
	unsigned char							m_strRecvBuffer[RECVBUF_SIZE];
	stack<CServAddr*>					m_pServers;
};

extern IDisplayLogger* g_pLogger;

#endif // CMASTER_QUERY_H