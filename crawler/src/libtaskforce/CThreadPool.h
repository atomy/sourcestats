#ifndef CTHREADPOOL_H
#define CTHREADPOOL_H

#include "CWorker.h"
#include <vector>
#include "CTask.h"
#include <pthread.h>
#include "CTaskForce.h"
#include "libcursesfrontend/CDisplayLogger.h"

using namespace std;

class CTask;
class CWorker;
class CTaskForce;

class CThreadPool
{
public:
  CThreadPool(CTaskForce*);
  ~CThreadPool(void);

public:
  void										init(void);
  void										SpawnThread(void);
	CWorker*								_SpawnThread(void);

  static	void						OnThreadExit(pthread_t);
  void										RemoveThread(pthread_t);
  bool										assignTask(CTask* t);
	void										OnTaskCompleted( CTask* pTask );
	IDisplayStats*					getStats();
	IDisplayLogger*					getLogger();
	int getNumIdleThreads();
private:
  vector <CWorker*> m_vWorkers;
	CTaskForce* m_pTaskForce;
	pthread_mutex_t muStatsChange;
	pthread_mutex_t muWorkers;
	int m_iIdleThreads;
};

struct CThreadArgs
{
  CWorker* pInfo;
};

#endif // CTHREADPOOL_H