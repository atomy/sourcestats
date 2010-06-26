#include "ThreadFactory.h"
#include <stdio.h>
#include <iostream>


void ThreadFactory::RemoveThread( ThreadedRequest* pThread )
{
	// TODO, iterator of pThread needed within m_vThreads
	//m_vThreads.erase( pThread );
}

void ThreadFactory::AddThread( ThreadedRequest* pThread )
{
	m_vThreads.push_back( pThread );
}

void ThreadFactory::CheckThreads( void )
{
	std::vector<ThreadedRequest*>::iterator it = m_vThreads.begin();

	while ( it < m_vThreads.end() )
	{
		ThreadedRequest* pThread = (*it);

		if ( pThread->GetStartTime() + pThread->GetTimeout() < time(NULL) )
		{
			int ret = pthread_cancel( pThread->GetThreadId() );
			
			if ( ret != 0 )
				std::cerr << "ThreadFactory::CheckThreads() ERROR while trying to cancel thread '" << pThread->GetThreadId() << "'" << std::endl;
#ifdef DEBUG
			else
				std::cout << "ThreadFactory::CheckThreads() thread '" << pThread->GetThreadId() << "' has been cancelled due timeout" << std::endl;
#endif
			 it = m_vThreads.erase( it );
		}
		else
			it++;
	}
}

int ThreadFactory::GetActiveThreadNo( void )
{
	return m_vThreads.size();
}
