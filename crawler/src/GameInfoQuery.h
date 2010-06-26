#ifndef GAMEINFOQUERY_H
#define GAMEINFOQUERY_H

#include "const.h"
#include <stdio.h>
#include "ThreadedRequest.h"
#include "GameserverInfo.h"

class GameStats;

class GameInfoQuery : public ThreadedRequest
{
public:
	GameInfoQuery( servAddr );

	void				Exec( void );
	GameserverInfo*		GetGSInfo() { return m_pGSInfo; }
	void				QueryforASINFO( void );
	query_state			GetStatus( void ) { return m_iState; }
	void				SetParent( GameStats* pStats ) { m_pParent = pStats; }

private:
	query_state			m_iState;
	GameserverInfo*		m_pGSInfo;
	servAddr			m_sAddr;
	GameStats*		    m_pParent;

	void				ParseASINFO( const char*, size_t );
};

#endif // GAMEINFOQUERY_H
