#include "CTaskForce.h"
#include "CTask.h"
#include "CTaskPool.h"
#include "CThreadPool.h"
#include "CDefaultTask.h"
#include "CTaskForceStats.h"
#include <sys/time.h>
#include <iostream>
#include "../sourcestats/timediff.h"
#include <stdio.h>

#define TASKFORCE_SLEEP_DEFAULT_MS 100


using namespace std;

CTaskForce::CTaskForce(const int maxTasks, const int maxThreads)
{
  m_pTaskPool = new CTaskPool(this);
	m_pThreadPool = new CThreadPool(this);

  pthread_mutex_init(&muStatsChange, NULL);
	pthread_mutex_init(&muLoggerChange, NULL);
	pthread_mutex_init(&m_muEventHandlers, NULL);

	InstallEventHandler(m_pTaskPool);
	InstallEventHandler(m_pThreadPool);
}

CTaskForce::~CTaskForce()
{
  delete m_pTaskPool;
	delete m_pThreadPool;	
}

bool CTaskForce::start()
{
	int ret = pthread_create( &m_iThread, NULL, &CTaskForce::init, this );
	if(ret == 0)
		return true;
	return false;
}

// thread entry point
void* CTaskForce::init(void* arg)
{
	CTaskForce* pThis = (CTaskForce*)arg;

	// go into the objects loop
	pThis->_init();
	pThis->run();

	return NULL;
}

void CTaskForce::run()
{
	m_bRun = true;
	CTimeDiff time;

	while(m_bRun) {
		time.setNow();

		// run
		update(time.getDiff());

		usleep(1000*TASKFORCE_SLEEP_DEFAULT_MS);
	}
}

void CTaskForce::update(long mtime)
{
	m_pTaskPool->assignTasks();
}

void CTaskForce::_init()
{
	m_pThreadPool->init();
	m_pTaskPool->init();

	// create threads till max
	while(m_pStats->getValue(THREADS_MAX) > m_pStats->getValue(THREADS_CURRENT)) {
		m_pThreadPool->SpawnThread();
	}
}

bool CTaskForce::AddTask(CTask* t)
{	
	if(getStats()->getValue(JOBS_WAITING) >= getStats()->getValue(JOBS_MAX)) {
		return false;
	}
	return m_pTaskPool->addTask(t);
}

void CTaskForce::setStats(IDisplayStats* pStats)
{
	pthread_mutex_lock(&muStatsChange); // LOCK
	m_pStats = pStats;
	pthread_mutex_unlock(&muStatsChange); // UNLOCK
}

IDisplayStats* CTaskForce::getStats()
{
	pthread_mutex_lock(&muStatsChange); // LOCK
	IDisplayStats* pStats = m_pStats;
	pthread_mutex_unlock(&muStatsChange); // UNLOCK

	return pStats;
}

void CTaskForce::setLogger(IDisplayLogger* pLogger)
{
	pthread_mutex_lock(&muLoggerChange); // LOCK
	m_pLogger = pLogger;
	pthread_mutex_unlock(&muLoggerChange); // UNLOCK
}

IDisplayLogger* CTaskForce::getLogger()
{
	pthread_mutex_lock(&muLoggerChange); // LOCK
	IDisplayLogger* pLogger = m_pLogger;
	pthread_mutex_unlock(&muLoggerChange); // UNLOCK

	return pLogger;
}

CTaskPool* CTaskForce::getTaskPool()
{
	return m_pTaskPool;
}


CThreadPool* CTaskForce::getThreadPool()
{
	return m_pThreadPool;
}

void CTaskForce::shutdown()
{
	m_bRun = false;
}

pthread_t CTaskForce::getThread()
{
	return m_iThread;
}

CTask* CTaskForce::getNextCompletedTask()
{
	return m_pTaskPool->getCompletedTask();
}

void CTaskForce::OnTaskStarted(CTask* pTask)
{
	pthread_mutex_lock(&m_muEventHandlers); // LOCK
	// call all registered eventhandlers
	vector<ITaskForceEventHandler*>::iterator it = m_pEventHandlers.begin();
	while(it < m_pEventHandlers.end()) {
		ITaskForceEventHandler* pHandler = (*it);
		if(!pHandler->IsValidEvent(pTask))
			continue;
		pHandler->OnTaskStarted(pTask);
		it++;
	}
	pthread_mutex_unlock(&m_muEventHandlers); // UNLOCK
}

void CTaskForce::OnTaskCompleted(CTask* pTask)
{
	pthread_mutex_lock(&m_muEventHandlers); // LOCK
	// call all registered eventhandlers
	vector<ITaskForceEventHandler*>::iterator it = m_pEventHandlers.begin();
	while(it < m_pEventHandlers.end()) {
		ITaskForceEventHandler* pHandler = (*it);
		pHandler->OnTaskCompleted(pTask);
		it++;
	}
	pthread_mutex_unlock(&m_muEventHandlers); // UNLOCK
}

void CTaskForce::InstallEventHandler(ITaskForceEventHandler* pHandler)
{
	pthread_mutex_lock(&m_muEventHandlers); // LOCK
	m_pEventHandlers.push_back(pHandler);
	pthread_mutex_unlock(&m_muEventHandlers); // UNLOCK
}