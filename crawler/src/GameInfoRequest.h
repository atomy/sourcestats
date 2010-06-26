#ifndef GAMESERVERREQUEST_H
#define GAMESERVERREQUEST_H

#include "const.h"
#include <stdio.h>
#include "GameserverInfo.h"


class GameInfoRequest
{
public:
	GameInfoRequest( servAddr );

	GameserverInfo*		GetGSInfo() { return m_pGSInfo; }
	void				QueryforASINFO( void );
	query_state			GetStatus( void ) { return m_iState; }

private:
	query_state			m_iState;
	GameserverInfo*		m_pGSInfo;
	servAddr			m_sAddr;

	void				ParseASINFO( const char*, size_t );
	GameserverInfo*		GetResult( void );
};

#endif // GAMESERVERREQUEST_H