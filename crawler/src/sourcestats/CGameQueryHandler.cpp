#include "CGameQueryHandler.h"
#include "CGameQueryTask.h"

#define CNAME "CGameQueryHandler"

CGameQueryHandler::CGameQueryHandler()
{

}

CGameQueryHandler::~CGameQueryHandler()
{

}


void CGameQueryHandler::OnTaskCompleted(CTask* pTask)
{
	g_pLogger->AddLog(CNAME, __FUNCTION__, "none");
}

void CGameQueryHandler::OnTaskStarted(CTask* pTask)
{
	g_pLogger->AddLog(CNAME, __FUNCTION__, "none");
}

bool CGameQueryHandler::IsValidEvent(CTask* pTask)
{
	CGameQueryTask* pGameTask = dynamic_cast<CGameQueryTask*>(pTask);
	if(pGameTask != NULL)
		return true;
	return false;
}

