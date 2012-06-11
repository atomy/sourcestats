#ifndef CTASKFORCE_H
#define CTASKFORCE_H

#include "CTaskForce.h"
#include "CThreadPool.h"
#include <pthread.h>
#include "CTaskForceStats.h"
#include "libcursesfrontend/CDisplayLogger.h"
#include "CTaskPool.h"
#include <vector>
#include "ITaskForceEventHandler.h"

#define TF_VERSION "0.0.1"

class CTask;
class CThreadPool;
class CTaskPool;

class CTaskForce
{
public:
	CTaskForce(const int maxTasks, const int maxThreads);
  ~CTaskForce();

	static void*												init(void* arg);

	// add a task to be run
	virtual	bool												AddTask(CTask*);

	virtual void												_init();
	virtual bool												start();
	virtual void												run();
	virtual void												update(long mtime);

	virtual void												setStats(IDisplayStats* pStats);
	virtual IDisplayStats*							getStats();

	virtual void												setLogger(IDisplayLogger* pLogger);
	virtual IDisplayLogger*							getLogger();

	virtual void												shutdown();
	virtual pthread_t										getThread();
	virtual CTask*											getNextCompletedTask();
	virtual CTaskPool*									getTaskPool();
	virtual CThreadPool*								getThreadPool();

	void																OnTaskStarted(CTask* pTask);
	void																OnTaskCompleted(CTask* pTask);	

	void																InstallEventHandler(ITaskForceEventHandler* pHandler);

private:
  CTaskPool*													m_pTaskPool;
	CThreadPool*												m_pThreadPool;
	IDisplayStats*											m_pStats;
	IDisplayLogger*											m_pLogger;

	pthread_mutex_t											muStatsChange;
	pthread_mutex_t											muLoggerChange;

//	int																	m_iMaxTasks;
//	int																	m_iMaxThreads;
	pthread_t														m_iThread;

	bool																m_bRun;

	vector<ITaskForceEventHandler*>			m_pEventHandlers;
	pthread_mutex_t											m_muEventHandlers;
};

#endif // CTASKFORCE_H