#ifndef THREADEDREQUEST_H
#define THREADEDREQUEST_H

#include "const.h"
#include <stdio.h>
#include <vector>
#include <pthread.h>
#include <time.h>
#include "ThreadFactory.h"


class ThreadedRequest : public ThreadFactory
{
public:
	ThreadedRequest( ThreadFactory* );
	         ~ThreadedRequest();

	time_t			GetStartTime( void ) { return m_tStartTime; }
	void            SetStartTime( time_t time ) { m_tStartTime = time; }
	pthread_t		GetThreadId( void ) { return m_tSelf; }
	int				GetTimeout( void ) { return m_iTimeout; }
	void			PreEntryPoint( void );
	void			PostEntryPoint( void );
	void			Init( void );

    virtual void    Log( const char* logMsg );
	virtual const char*   GetClassName( void ) = 0;

	virtual void	Loop( void );

	void			SetTimeout( int timeout ) { m_iTimeout = timeout; }
	void 			SetKill( bool );
	bool			GetKill( void ) { return m_bDie; }

	virtual bool	IsMasterquery( void ) { return false; }
	virtual bool 	IsGameInfoQuery( void ) { return false; }

protected:
	time_t			m_tStartTime;
    pthread_t	 	m_tSelf;
	int				m_iTimeout;
	ThreadFactory*  m_pFactory;
	bool			m_bDie;
};

#endif // THREADEDREQUEST_H
