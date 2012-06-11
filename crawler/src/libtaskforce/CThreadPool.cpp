#include "CThreadPool.h"
#include "CWorker.h"
#include <pthread.h>
//#include "config.h"
#include <iostream>
#include <typeinfo>
#include <string>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include "CTaskForceStats.h"
#include <stdio.h>
#include <iostream>

using namespace std;

CThreadPool::CThreadPool(CTaskForce* pForce) : m_pTaskForce(pForce)
{
	pthread_mutex_init(&muStatsChange, NULL);
	pthread_mutex_init(&muWorkers, NULL);
}

CThreadPool::~CThreadPool()
{
}

void CThreadPool::init(void)
{
}

CWorker* CThreadPool::_SpawnThread()
{
	CWorker* pWorker = new CWorker(); // Thread::EntryPoint() takes care of freeing that memory
	pWorker->mThreadArg = pWorker;
	pWorker->mThreadEntry = &CWorker::EntryPoint;

	// create thread
	int ret = pthread_create( &pWorker->mThreadId, &pWorker->mThreadAttr, pWorker->mThreadEntry, pWorker->mThreadArg );

	// if success do some logging etc, if not kill everything we built up
	if(ret == 0) {      
		pWorker->setTask(NULL);
		return pWorker;
	} else {
		delete pWorker;
	}
	return NULL;	
}

void CThreadPool::SpawnThread()
{
	if(getStats()->getValue(THREADS_CURRENT) >= getStats()->getValue(THREADS_MAX)) {
		getLogger()->AddLog("CThreadPool", __FUNCTION__, "max threads reached, thread not created!");
		return;
	}

	CWorker* pWorker = _SpawnThread();

	if(pWorker == NULL) {
		getLogger()->AddLog("CThreadPool", "SpawnThread", "error while spawning thread");
		return;
	}

	pthread_mutex_lock(&muWorkers); // LOCK	
	m_vWorkers.push_back(pWorker);
	pthread_mutex_unlock(&muWorkers); // UNLOCK

	getStats()->changeValueBy(THREADS_CURRENT, 1);
	getStats()->changeValueBy(THREADS_IDLE, 1);

	char log[64];
	snprintf(log, 64, "thread '%lu' spawned!", pWorker->mThreadId);
	getLogger()->AddLog("CThreadPool", "SpawnThread", log);
}

// Warning, this has to be thread-safe!!
// TODO, add some listeners here
void CThreadPool::OnThreadExit(pthread_t threadId)
{
	char log[64];
	snprintf(log, 64, "thread '%lu' exited.", threadId);	
	g_pTaskForce->getLogger()->AddLog("CThreadPool", __FUNCTION__, log);

	g_pTaskForce->getThreadPool()->RemoveThread(threadId);
}

void CThreadPool::RemoveThread(pthread_t threadId)
{
	vector<CWorker*>::iterator rmIt;
	pthread_mutex_lock(&muWorkers);  // LOCK
	vector<CWorker*>::iterator it = m_vWorkers.begin();
	CWorker* pDeleteWorker = (*it);
	while ( it < m_vWorkers.end() )
	{
		pDeleteWorker = (*it);
		if(pDeleteWorker->mThreadId == threadId)
			rmIt = it;
		it++;
	}
	if(rmIt >= m_vWorkers.begin()) {
		//    char log[64];
		//    snprintf(log, 64, "found thread and removed it! ('%lu')", (*rmIt)->mThreadId);
		//    m_pTaskForce->getLogger()->AddLog("CThreadPool", "RemoveThread", log);
		m_vWorkers.erase(rmIt);
		if(pDeleteWorker->getTask() == NULL) {
			getStats()->changeValueBy(THREADS_IDLE, -1);
		} else {
			getStats()->changeValueBy(THREADS_ASSIGNED, -1);
			delete pDeleteWorker->getTask(); // we are loosing the half-finished task here
		}
		getStats()->changeValueBy(THREADS_CURRENT, -1);
		delete pDeleteWorker;
	}
	pthread_mutex_unlock(&muWorkers);  // UNLOCK
}

// assign a given task to a free worker
bool CThreadPool::assignTask(CTask* t)
{
	bool success = false;
	pthread_mutex_lock(&muWorkers); // LOCK
	vector<CWorker*>::iterator it = m_vWorkers.begin();
	while(it < m_vWorkers.end()) {
		CWorker* pWorker = (*it);
		if(pWorker->getTask() == NULL) { // THREAD IDLE
			getLogger()->AddLog("CThreadPool", __FUNCTION__, "assigning task now!");
			pWorker->setTask(t);
			t->OnTaskAssigned(pWorker);

			getStats()->changeValueBy(THREADS_ASSIGNED, 1);
			getStats()->changeValueBy(THREADS_IDLE, -1);

			success = true;
			break;
		}
		it++;
	}
	pthread_mutex_unlock(&muWorkers); // UNLOCK
	return success;
}

void CThreadPool::OnTaskCompleted( CTask* pTask )
{
	CWorker* pWorker = pTask->getWorker();

	getStats()->changeValueBy(THREADS_IDLE, 1);
	getStats()->changeValueBy(THREADS_ASSIGNED, -1);

	pWorker->setTask(NULL);
}

void CThreadPool::OnTaskStarted( CTask* pTask )
{ }

IDisplayStats* CThreadPool::getStats()
{
	return m_pTaskForce->getStats();
}

IDisplayLogger* CThreadPool::getLogger()
{
	return m_pTaskForce->getLogger();
}

int CThreadPool::getNumIdleThreads()
{
	return m_iIdleThreads;
}

bool CThreadPool::IsValidEvent( CTask* pTask )
{
	return true;
}
