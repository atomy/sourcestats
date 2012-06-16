#ifndef CGAME_QUERY_H
#define CGAME_QUERY_H

#include "CServAddr.h"
#include <vector>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include "libcursesfrontend/IDisplayLogger.h"
#include <stack>
#include "CInfoStruct.h"

#define RECVBUF_SIZE 8192
#define SENDBUFFER_SIZE 128

using namespace std;

class CGameQuery
{
public:
	CGameQuery(const char* strHostname, const char* strPort);
	~CGameQuery();

	bool											init();

	void											doRequest();
	int												buildSendBuffer(unsigned char* buf, size_t size);
	void											retrieveServerInfo();
	CInfoStruct&							getInfoStruct();

private:
	void											parseData();

	const char*								m_strQueryTargetHostname;
	const char*								m_strQueryTargetPort;
	int												m_iSocket;
	int												m_iRecvBytes, m_iSentBytes;
	struct addrinfo						*m_pServerinfo, *m_pSelectedServ;
	unsigned char							m_strSendBuffer[SENDBUFFER_SIZE];
	unsigned char							m_strRecvBuffer[RECVBUF_SIZE];
	CInfoStruct								m_infoResult;
	CServAddr									m_targetServer;
};

extern IDisplayLogger*			g_pLogger;

#endif // CGAME_QUERY_H