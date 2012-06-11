#include <iostream>
#include "const.h"
#include "ThreadedRequest.h"
#include <time.h>
#include "ThreadFactory.h"
#include <string.h>

extern pthread_mutex_t muLog;

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

void ThreadedRequest::PreEntryPoint( void )
{
}

void ThreadedRequest::Init( void )
{
	m_tSelf = pthread_self();
	m_pFactory->AddThread( this );	
}

void ThreadedRequest::PostEntryPoint( void )
{
	Loop();
}

void ThreadedRequest::SetKill( bool bKillme )
{
	m_bDie = bKillme;
}

void ThreadedRequest::Log( const char* logMsg )
{
    pthread_mutex_lock (&muLog);
	std::cout << "[" << time(NULL) << "][THREAD|" << GetThreadId() << "] " << logMsg << std::endl;
    pthread_mutex_unlock (&muLog);
}

void ThreadedRequest::Loop( void )
{
	while(1)
	{
		sleep(1);
	}
}