#include "ThreadFactory.h"
#include <stdio>

using namespace std;

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
	vector<ThreadedRequest*>::iterator it = m_vThreads.begin();

	while ( it < m_vThreads.end(); )
	{
		ThreadedRequest* pThread = (*it);

		if ( pThread->GetStartTime() + pThread->GetTimeout() < time() )
		{
			int ret = pthread_cancel( pThread->GetThreadId() )
			
			if ( ret != 0 )
				cerr << "ThreadFactory::CheckThreads() ERROR while trying to cancel thread '" << pThread->GetThreadId() << "'" << endl;
#ifdef DEBUG
			else
				cout << "ThreadFactory::CheckThreads() thread '" << pThread->GetThreadId() << "' has been cancelled due timeout" << endl;
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