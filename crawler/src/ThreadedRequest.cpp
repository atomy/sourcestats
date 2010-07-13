#include <iostream>
#include "const.h"
#include "ThreadedRequest.h"
#include <time.h>
#include "ThreadFactory.h"
#include <string.h>

ThreadedRequest::ThreadedRequest( ThreadFactory* pFactory )
{
    strncpy( m_sParentClassName, "UNKNOWN", 32 );
    m_pFactory = pFactory;
	m_tStartTime = time(NULL);
	m_bDie = false;
}

ThreadedRequest::~ThreadedRequest()
{
}

void ThreadedRequest::ThreadExit( void )
{
    Log("ThreadedRequest::ThreadExit() thread exited.");
    pthread_exit(0);
}

void ThreadedRequest::EntryPoint( void )
{
	m_tSelf = pthread_self();
    m_pFactory->AddThread( this );
}

void ThreadedRequest::SetKill( bool bKillme )
{
	m_bDie = bKillme;
}

extern pthread_mutex_t muLog;

void ThreadedRequest::Log( const char* logMsg )
{
    pthread_mutex_lock (&muLog);
	std::cout << "[" << time(NULL) << "][THREAD|" << GetThreadId() << "|P|" << GetParentClassName() << "] " << logMsg << std::endl;
    pthread_mutex_unlock (&muLog);
}
