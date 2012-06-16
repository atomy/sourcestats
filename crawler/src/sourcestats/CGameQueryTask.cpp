#include "CGameQueryTask.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <netdb.h>
#include <errno.h>
#include <string>
#include <iostream>
#include <sstream>
#include <iomanip>
#include "CMasterQuery.h"

using namespace std;

// TODO, get rid of it
#define HOSTNAME "74.82.47.38"
#define PORT "27025"

#define CNAME "CGameQueryTask"

CGameQueryTask::CGameQueryTask() : CTask()
{
	m_pQuery = new CGameQuery(HOSTNAME, PORT);
}

CGameQueryTask::~CGameQueryTask()
{
	delete m_pQuery;
}

void CGameQueryTask::run()
{
	// callback
	OnTaskStarted();

	g_pLogger->AddLog(CNAME, __FUNCTION__, "doing stuff...");

	if(m_pQuery->init())
		m_pQuery->doRequest();
	else
		g_pLogger->AddLog(CNAME, __FUNCTION__, "error while CGameQuery::init()");

	g_pLogger->AddLog(CNAME, __FUNCTION__, "done with stuff!");

	// callback
	OnTaskCompleted();
}

CGameQuery* CGameQueryTask::getQuery()
{
	return m_pQuery;
}
