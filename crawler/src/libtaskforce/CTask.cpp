#include "CTask.h"

CTask::CTask()
{
	m_pWorker = NULL;
}

void CTask::run()
{
}

void CTask::OnTaskAssigned(CWorker* pWorker)
{
  m_pWorker = pWorker;
}

void CTask::OnTaskCompleted()
{
  g_pTaskForce->OnTaskCompleted(this);
}

void CTask::OnTaskStarted()
{
  g_pTaskForce->OnTaskStarted(this);
}