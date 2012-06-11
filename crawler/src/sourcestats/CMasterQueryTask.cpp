#include "CMasterQueryTask.h"
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
#define HOSTNAME "208.64.200.39"
#define PORT "27011"

CMasterQueryTask::CMasterQueryTask() : CTask()
{
	m_pQuery = new CMasterQuery(HOSTNAME, PORT);
}

CMasterQueryTask::~CMasterQueryTask()
{
	delete m_pQuery;
}

void CMasterQueryTask::run()
{
	OnTaskStarted();
	
	g_pLogger->AddLog("CMasterQueryTask", __FUNCTION__, "doing stuff...");
	
	if(m_pQuery->init())
		m_pQuery->doRequest();
	else
		g_pLogger->AddLog("CMasterQueryTask", __FUNCTION__, "error while CMasterQuery::init()");

	g_pLogger->AddLog("CMasterQueryTask", __FUNCTION__, "done with stuff!");

	// callback
	OnTaskCompleted();
}