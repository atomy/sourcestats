#ifndef CTASKPOOL_H
#define CTASKPOOL_H

#include <pthread.h>
#include <queue>
#include "CTask.h"
#include "CThreadPool.h"
#include "CTaskForceStats.h"

using namespace std;

class CTask;
class CThreadPool;
class CTaskForce;

class CTaskPool
{
public:
  CTaskPool(CTaskForce* pForce);
  ~CTaskPool();

  void									init();

  bool									addTask(CTask* t);
  void									assignTasks();
  //void									handleCompletedTasks();
	CTask*								getCompletedTask();

  void									OnTaskCompleted(CTask*);
  void									OnTaskStarted(CTask*);
	IDisplayStats*				getStats();

private:
  CTaskForce*						m_pTaskForce;

  queue<CTask*>					mWaitingTasks;
  queue<CTask*>					mDoneTasks;

	pthread_mutex_t				muWaitingTasks;
	pthread_mutex_t				muDoneTasks;

	int										m_iJobsWaiting;
};

#endif // CTASKPOOL_H