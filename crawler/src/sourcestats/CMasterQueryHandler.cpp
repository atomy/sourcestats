#include "CMasterQueryHandler.h"
#include "CMasterQueryTask.h"
#include "CGameQueryTask.h"

#define CNAME "CMasterQueryHandler"


CMasterQueryHandler::CMasterQueryHandler()
{

}

CMasterQueryHandler::~CMasterQueryHandler()
{

}


void CMasterQueryHandler::OnTaskCompleted(CTask* pTask)
{
  g_pLogger->AddLog(CNAME, __FUNCTION__, "master query completed!");
}

void CMasterQueryHandler::OnTaskStarted(CTask* pTask)
{
  g_pLogger->AddLog(CNAME, __FUNCTION__, "master query started!");
}

bool CMasterQueryHandler::IsValidEvent(CTask* pTask)
{
	CMasterQueryTask* pMasterTask = dynamic_cast<CMasterQueryTask*>(pTask);
	if(pMasterTask != NULL)
		return true;
	return false;
}

