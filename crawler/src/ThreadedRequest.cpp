#include <iostream>
#include "const.h"
#include "ThreadedRequest.h"
#include <time.h>

ThreadedRequest::ThreadedRequest( void )
{
    m_iState = STATE_NEW;
	m_tStartTime = time();
}

void ThreadedRequest::InitThread( void )
{
    m_tSelf = pthread_self();
}
