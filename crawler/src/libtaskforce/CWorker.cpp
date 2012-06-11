#include "CWorker.h"
#include <unistd.h>

CWorker::CWorker()
{
	mThreadEntry = NULL;
	mThreadArg = NULL;
	m_pTask = NULL;
	//iState = THREAD_IDLE;
	pthread_attr_init(&mThreadAttr);
	pthread_mutex_init(&muTaskChange, NULL);
}

CWorker::~CWorker(void)
{
}

void* CWorker::EntryPoint(void* arg)
{
  CWorker* pThis = (CWorker*)arg;

	// go into the objects loop
	pThis->run();

	// we are exiting, clean up
	CThreadPool::OnThreadExit(pthread_self());
	delete pThis;

	return NULL;
}

// run() example, uglyness doesnt matter!
void CWorker::run(void)
{
  while(1) {    
    if(m_pTask == NULL)
      usleep(1000*WORKER_SLEEP_IDLE_MS);
    else
      m_pTask->run();
  }
}

void CWorker::setTask( CTask* pTask )
{
	pthread_mutex_lock(&muTaskChange); // LOCK
	m_pTask = pTask;
	pthread_mutex_unlock(&muTaskChange); // UNLOCK
}

CTask* CWorker::getTask()
{
	pthread_mutex_lock(&muTaskChange); // LOCK
	CTask* pTask = m_pTask;
	pthread_mutex_unlock(&muTaskChange); // UNLOCK

	return pTask;
}
