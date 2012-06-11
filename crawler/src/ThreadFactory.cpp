#include "ThreadFactory.h"
#include <stdio.h>
#include <iostream>
#include <string.h>
#include <assert.h>
#include <errno.h>
#include "ThreadedRequest.h"
#include <signal.h>

extern pthread_mutex_t muLog;

ThreadFactory::ThreadFactory()
{
	pthread_mutex_init(&m_threadMutex, NULL);
	strncpy( m_sParentClassName, "UNKNOWN", 32 );
}

void ThreadFactory::AddThread( ThreadedRequest* pThread )
{
    //assert(strcmp(GetParentClassName(), "UNKNOWN")); // != UNKNOWN
    assert(pThread->GetThreadId());
	char logout[128];
	snprintf(logout, 128, "ThreadFactory::AddThread() added new thread: '%u' starttime: '%u' timeout: '%d'", (unsigned int)pThread->GetThreadId(), (unsigned int)pThread->GetStartTime(), pThread->GetTimeout());
	Log(logout);
    pthread_mutex_lock(&m_threadMutex);
	m_vThreads.push_back( pThread );
    pthread_mutex_unlock(&m_threadMutex);
}

void ThreadFactory::CheckThreads( void )
{
	char logout[128];
	snprintf(logout, 128, "ThreadFactory::CheckThreads() checking '%d' threads", m_vThreads.size());
	Log(logout);

    pthread_mutex_lock(&m_threadMutex);
	std::vector<ThreadedRequest*>::iterator it = m_vThreads.begin();

	while ( it < m_vThreads.end() )
	{
		ThreadedRequest* pThread = (*it);
		int ret = CheckThread(pThread);

		if ( pThread->GetKill() )
		{
			char logout[128];
			snprintf(logout, 128, "ThreadFactory::CheckThreads() thread '%u' has killswitch enabled, cancelling...", (unsigned int)pThread->GetThreadId());
			LogNoDeadLock(logout);
			CancelThread((*it));
			it = RemoveThread(it);
		}
		else if ( ret != 0 && errno == ESRCH )
		{
			char logout[128];
			snprintf(logout, 128, "ThreadFactory::CheckThreads() thread '%u' does no longer exists", (unsigned int)pThread->GetThreadId());
			LogNoDeadLock(logout);
			it = RemoveThread(it);
		}
		else if ( pThread->GetStartTime() + pThread->GetTimeout() < time(NULL) )
		{
			TimeoutThread_Callback( pThread );
			ret = CancelThread( pThread );
			if ( ret != 0 )
				std::cerr << "[" << time(NULL) << "] ThreadFactory::CheckThreads() ERROR while trying to cancel thread '" << pThread->GetThreadId() << "' -- " << strerror(errno) << std::endl;
			else
			{
				char logout[128];
				snprintf(logout, 128, "ThreadFactory::CheckThreads() thread '%u' has been cancelled due timeout", (unsigned int)pThread->GetThreadId());
				LogNoDeadLock(logout);
			}
			it = RemoveThread(it);
		}
		else
			it++;
	}
    pthread_mutex_unlock(&m_threadMutex);
}

std::vector<ThreadedRequest*>::iterator ThreadFactory::RemoveThread( std::vector<ThreadedRequest*>::iterator pIt )
{
	char logout[128];
	snprintf(logout, 128, "ThreadFactory::RemoveThread() removing thread '%u'", (unsigned int)(*pIt)->GetThreadId());
	ExitThread_Callback((*pIt));
	LogNoDeadLock(logout);
	std::vector<ThreadedRequest*>::iterator it = m_vThreads.erase( pIt );
	return it;
}

int ThreadFactory::CancelThread( ThreadedRequest* pRequest )
{
	return pthread_cancel( pRequest->GetThreadId() );
}

int ThreadFactory::CheckThread( ThreadedRequest* pRequest )
{
	return pthread_kill( pRequest->GetThreadId(), 0 );
}

// count my threads and any subsequent threadfactories' threads
int ThreadFactory::GetActiveThreadNo( void )
{
//#ifdef DEBUG
//    std::cout << "[" << time(NULL) << "][P|" << GetParentClassName() << "] ThreadFactory::GetActiveThreadNo() counting child-threads, vector size: '" << m_vThreads.size() << "'" << std::endl;
//#endif
	//std::cout << "ThreadFactory::GetActiveThreadNo() LOCK" << std::endl;
    pthread_mutex_lock(&m_threadMutex);
    int iThreadCount = 0;

    if ( m_vThreads.size() <= 0 )
	{
		pthread_mutex_unlock(&m_threadMutex);
        return iThreadCount;
	}

    std::vector<ThreadedRequest*>::iterator it = m_vThreads.begin();

	while ( it < m_vThreads.end() )
	{
	    ThreadedRequest* pThread = (*it);
	    iThreadCount += pThread->GetActiveThreadNo();
        it++;
        iThreadCount++;
	}
    pthread_mutex_unlock(&m_threadMutex);
	//std::cout << "ThreadFactory::GetActiveThreadNo() UNLOCK" << std::endl;
	
	return iThreadCount;
}

//void ThreadFactory::ExitCallback( ThreadedRequest* pThread )
//{
//#ifdef DEBUG
//    std::cout << "[" << time(NULL) << "][P|" << GetParentClassName() << "] ThreadFactory::ExitCallback() exit callback from thread: '" << pThread->GetThreadId() << "'" << std::endl;
//#endif
//    RemoveThread( pThread );
//}

//#ifdef DEBUG
//void ThreadFactory::SetParentClassName( const char* pName )
//{
//	strncpy( m_sParentClassName, pName, 32 );
//}
//
//const char* ThreadFactory::GetParentClassName( void )
//{
//	return m_sParentClassName;
//}
//#endif

void ThreadFactory::Log( const char* logMsg )
{
    pthread_mutex_lock (&muLog);
	std::cout << "[" << time(NULL) << "] " << logMsg << std::endl;
    pthread_mutex_unlock (&muLog);
}

void ThreadFactory::TimeoutThread_Callback( ThreadedRequest* pThread )
{
	ExitThread_Callback(pThread);
}

void ThreadFactory::ExitThread_Callback( ThreadedRequest* pThread )
{
}
