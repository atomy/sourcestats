#ifndef CWORKER_H
#define CWORKER_H

#include <pthread.h>
#include "CTask.h"

#define WORKER_SLEEP_IDLE_MS 100

class CTask;

enum ThreadState {
	THREAD_IDLE,
	THREAD_TASK_ASSIGNED
};

class CWorker
{
public:
	CWorker();
	~CWorker();

	static	void*		EntryPoint(void*);

	void						run();
	CTask*					getTask();
  void						setTask(CTask* pTask);

public:
	pthread_t				mThreadId;
	pthread_attr_t	mThreadAttr;

	void*						(*mThreadEntry)(void*);	
	void*						mThreadArg;
	//ThreadState			iState; // We dont really need this, yet

private:
	CTask*					m_pTask;
  pthread_mutex_t muTaskChange;
};

#endif // CWORKER_H