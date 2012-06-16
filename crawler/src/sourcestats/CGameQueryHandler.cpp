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
	CGameQueryTask* pQueryTask = dynamic_cast<CGameQueryTask*>(pTask);
	if(pQueryTask) {
		char buf[128];
		CInfoStruct queryInfo = pQueryTask->getQuery()->getInfoStruct();
		queryInfo.toString(buf, 128);
		g_pLogger->AddLog(CNAME, __FUNCTION__, buf);
	} else {
		g_pLogger->AddLog(CNAME, __FUNCTION__, "error retrieving result of query!");
	}
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

