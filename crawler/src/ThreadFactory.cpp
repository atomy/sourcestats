#include "ThreadFactory.h"
#include <stdio.h>
#include <iostream>
#include <string.h>
#include <assert.h>
#include <errno.h>
#include "ThreadedRequest.h"
#include <signal.h>

ThreadFactory::ThreadFactory()
{
	pthread_mutex_init(&threadMutex, NULL);
	strncpy( m_sParentClassName, "UNKNOWN", 32 );
}

void ThreadFactory::RemoveThread( ThreadedRequest* pThread )
{
	// TODO, iterator of pThread needed within m_vThreads - really? no smarter way?
	std::vector<ThreadedRequest*>::iterator it = m_vThreads.begin();

	while ( it < m_vThreads.end() )
	{
	    if ( (*it) == pThread )
	    {
	        // TODO, lock
            m_vThreads.erase( it );
            // TODO, unlock
			char logout[128];
			snprintf(logout, 128, "ThreadFactory::RemoveThread() erased thread: '%u'", (unsigned int)(*it)->GetThreadId());
            delete (*it);
            break;
	    }
	    else
            it++;
	}
}

void ThreadFactory::AddThread( ThreadedRequest* pThread )
{
    assert(strcmp(GetParentClassName(), "UNKNOWN")); // != UNKNOWN
    assert(pThread->GetThreadId());
	char logout[128];
	snprintf(logout, 128, "ThreadFactory::AddThread() added new thread: '%u' starttime: '%u' timeout: '%d'", (unsigned int)pThread->GetThreadId(), (unsigned int)pThread->GetStartTime(), pThread->GetTimeout());
	Log(logout);
	m_vThreads.push_back( pThread );

//#ifdef DEBUG
//    std::cout << "[" << time(NULL) << "][P|" << GetParentClassName() << "] ThreadFactory::AddThread() thread count: " << m_vThreads.size() << std::endl;
//#endif
}

void ThreadFactory::CheckThreads( void )
{
	std::vector<ThreadedRequest*>::iterator it = m_vThreads.begin();

	while ( it < m_vThreads.end() )
	{
		ThreadedRequest* pThread = (*it);
		int ret = pthread_kill( pThread->GetThreadId(), 0 );

		if ( pThread->GetKill() )
		{
			ThreadExit( pThread );
			it = m_vThreads.erase( it );
		}
		else if ( ret != 0 && errno == ESRCH )
		{
			char logout[128];
			snprintf(logout, 128, "ThreadFactory::CheckThreads() thread '%u' does no longer exists", (unsigned int)pThread->GetThreadId());
			Log(logout);
			ThreadExit( (*it) );
			it = m_vThreads.erase( it );
		}
		else if ( pThread->GetStartTime() + pThread->GetTimeout() < time(NULL) )
		{
			ret = pthread_cancel( pThread->GetThreadId() );

			if ( ret != 0 )
				std::cerr << "[" << time(NULL) << "][P|" << GetParentClassName() << "] ThreadFactory::CheckThreads() ERROR while trying to cancel thread '" << pThread->GetThreadId() << "' -- " << strerror(errno) << std::endl;
			else
			{
				char logout[128];
				snprintf(logout, 128, "ThreadFactory::CheckThreads() thread '%u' has been cancelled due timeout", (unsigned int)pThread->GetThreadId());
				Log(logout);
			}

			ThreadExit( (*it) );
			it = m_vThreads.erase( it );
		}
		else
			it++;
	}
}

void ThreadFactory::ThreadExit( ThreadedRequest* pRequest )
{
	delete pRequest;
}

// count my threads and any subsequent threadfactories' threads
int ThreadFactory::GetActiveThreadNo( void )
{
//#ifdef DEBUG
//    std::cout << "[" << time(NULL) << "][P|" << GetParentClassName() << "] ThreadFactory::GetActiveThreadNo() counting child-threads, vector size: '" << m_vThreads.size() << "'" << std::endl;
//#endif
    int iThreadCount = 0;

    if ( m_vThreads.size() <= 0 )
        return iThreadCount;

    std::vector<ThreadedRequest*>::iterator it = m_vThreads.begin();

	while ( it < m_vThreads.end() )
	{
	    ThreadedRequest* pThread = (*it);
	    iThreadCount += pThread->GetActiveThreadNo();
        it++;
        iThreadCount++;
	}

	return iThreadCount;
}

//void ThreadFactory::ExitCallback( ThreadedRequest* pThread )
//{
//#ifdef DEBUG
//    std::cout << "[" << time(NULL) << "][P|" << GetParentClassName() << "] ThreadFactory::ExitCallback() exit callback from thread: '" << pThread->GetThreadId() << "'" << std::endl;
//#endif
//    RemoveThread( pThread );
//}

#ifdef DEBUG
void ThreadFactory::SetParentClassName( const char* pName )
{
	strncpy( m_sParentClassName, pName, 32 );
}

const char* ThreadFactory::GetParentClassName( void )
{
	return m_sParentClassName;
}
#endif

extern pthread_mutex_t muLog;

void ThreadFactory::Log( const char* logMsg )
{
    pthread_mutex_lock (&muLog);
	std::cout << "[" << time(NULL) << "] " << logMsg << std::endl;
    pthread_mutex_unlock (&muLog);
}
