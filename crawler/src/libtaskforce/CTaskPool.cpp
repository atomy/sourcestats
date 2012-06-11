#include "CTaskPool.h"
#include "CTask.h"
#include "CTaskForceStats.h"
#include <stdio.h>
#include <iostream>

using namespace std;

#define class

CTaskPool::CTaskPool(CTaskForce* pForce) : m_pTaskForce(pForce)
{
}

CTaskPool::~CTaskPool()
{
}

void CTaskPool::init()
{
	pthread_mutex_init(&muWaitingTasks, NULL);
	pthread_mutex_init(&muDoneTasks, NULL);
}

IDisplayStats* CTaskPool::getStats()
{
  return m_pTaskForce->getStats();
}

bool CTaskPool::addTask(CTask* t)
{
	pthread_mutex_lock(&muWaitingTasks); // LOCK

  mWaitingTasks.push(t);
	getStats()->changeValueBy(JOBS_WAITING, 1);

  pthread_mutex_unlock(&muWaitingTasks); // UNLOCK

	return true;
}

void CTaskPool::assignTasks()
{
	// no workers available
	if(getStats()->getValue(THREADS_IDLE) == 0)
		return;

	// no jobs waiting
	if(getStats()->getValue(JOBS_WAITING) == 0)
		return;

  char log[128];
	snprintf(log, 128, "assigning tasks... (pool size: %d)", mWaitingTasks.size());
	g_pTaskForce->getLogger()->AddLog("CTaskPool", __FUNCTION__, log);

  pthread_mutex_lock(&muWaitingTasks); // LOCK
  while(!mWaitingTasks.empty()) {
    CTask* t = mWaitingTasks.front();
    if(g_pTaskForce->getThreadPool()->assignTask(t)) {
      mWaitingTasks.pop();
			getStats()->changeValueBy(JOBS_WAITING, -1);
    } else {
			// cant assign any tasks right now for whatever reason
			break;
		}
  }
  pthread_mutex_unlock(&muWaitingTasks); // UNLOCK
}

void CTaskPool::OnTaskCompleted(CTask* pTask)
{
  m_pTaskForce->getLogger()->AddLog("CThreadPool", __FUNCTION__, "task completed!");
  pthread_mutex_lock(&muDoneTasks); // LOCK
  mDoneTasks.push(pTask);
  pthread_mutex_unlock(&muDoneTasks); // UNLOCK

	//m_pTaskForce->getThreadPool()->OnTaskCompleted(pTask);
}

void CTaskPool::OnTaskStarted(CTask* pTask)
{
  m_pTaskForce->getLogger()->AddLog("CThreadPool", __FUNCTION__, "task started!");
}

//void CTaskPool::handleCompletedTasks()
//{
//  pthread_mutex_lock(&muDoneTasks); // LOCK
//
//  while(!mDoneTasks.empty()) {
//    CTask* t = mDoneTasks.front();
//    mDoneTasks.pop();
//    delete(t);
//  }
//
//  pthread_mutex_unlock(&muDoneTasks); // UNLOCK
//}

CTask* CTaskPool::getCompletedTask()
{
	CTask* pTask = NULL;

	pthread_mutex_lock(&muDoneTasks); // LOCK
	
	if(!mDoneTasks.empty()) {
	  pTask = mDoneTasks.front();
		mDoneTasks.pop();
	}

	pthread_mutex_unlock(&muDoneTasks); // UNLOCK

	return pTask;
}

bool CTaskPool::IsValidEvent( CTask* pTask )
{
	return true;
}
