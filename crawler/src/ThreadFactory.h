#ifndef THREADFACTORY_H
#define THREADFACTORY_H

#include "const.h"
#include <stdio.h>
#include <vector>
#include <string.h>
#include "DebugLog.h"

class ThreadedRequest;

class ThreadFactory : public DebugLog
{
public:
    ThreadFactory();
    virtual						~ThreadFactory() { }

    virtual void                Log( const char* logMsg );
	void						AddThread( ThreadedRequest* pRequest );
	void						CheckThreads( void );
	int							GetActiveThreadNo( void );
	void						RemoveThread( ThreadedRequest* pThread );
	void 						ThreadExit( ThreadedRequest* pRequest );

//	virtual	void                ExitCallback( ThreadedRequest* pThread );
//	virtual void                FinishCallback( ThreadedRequest* pThread ) { }

	//virtual const char*         GetName( void ) { return "UNDEFINED"; }//= 0;
#ifdef DEBUG
	void			SetParentClassName( const char* );
	const char*		GetParentClassName( void );
#endif

private:


protected:
#ifdef DEBUG
	char			m_sParentClassName[64];
#endif
	std::vector<ThreadedRequest*>	m_vThreads;
	pthread_mutex_t					m_threadMutex; 
};

#endif // THREADFACTORY_H
