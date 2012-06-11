#include "CMasterQueryHandler.h"
#include "CMasterQueryTask.h"

CMasterQueryHandler::CMasterQueryHandler()
{

}

CMasterQueryHandler::~CMasterQueryHandler()
{

}


void CMasterQueryHandler::OnTaskCompleted(CTask* pTask)
{
	g_pLogger->AddLog("CThreadPool", __FUNCTION__, "in china ist ein sack reis umgefallen!");
}

void CMasterQueryHandler::OnTaskStarted(CTask* pTask)
{
  g_pLogger->AddLog("CThreadPool", __FUNCTION__, "in china ist ein sack reis aufgestanden!");
}

bool CMasterQueryHandler::IsValidEvent( CTask* pTask )
{
	CMasterQueryTask* pMasterTask = dynamic_cast<CMasterQueryTask*>(pTask);
	if(pMasterTask != NULL)
		return true;
	return false;
}

