#ifndef THREADFACTORY_H
#define THREADFACTORY_H

#include "const.h"
#include <stdio.h>
#include <vector>
#include "GameInfoQuery.h"
//#include "ThreadedRequest.h"


class ThreadFactory
{
public:
    //ThreadFactory() { }
    //~ThreadFactory() { }

	void						AddThread( ThreadedRequest* pRequest );
	void						CheckThreads( void );
	int							GetActiveThreadNo( void );
	void						RemoveThread( ThreadedRequest* pThread );

private:
	std::vector<ThreadedRequest*>	m_vThreads;
};

#endif // THREADFACTORY_H
