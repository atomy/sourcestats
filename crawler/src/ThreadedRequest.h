#ifndef THREADEDREQUEST_H
#define THREADEDREQUEST_H

#include "const.h"
#include <stdio.h>
#include <vector>
#include <pthread.h>
#include <time.h>


class ThreadedRequest
{
public:
	ThreadedRequest();

	time_t			GetStartTime( void ) { return m_tStartTime; }
	void            SetStartTime( time_t time ) { m_tStartTime = time; }
	pthread_t		GetThreadId( void ) { return m_tSelf; }
	int				GetTimeout( void ) { return m_iTimeout; }
    void            InitThread( void );

	//virtual void*   GetParent( void ) = 0;

	void			SetTimeout( int timeout ) { m_iTimeout = timeout; }

private:
	time_t			m_tStartTime;
    pthread_t	 	m_tSelf;
    query_state 	m_iState;
	int				m_iTimeout;
};

#endif // THREADEDREQUEST_H
