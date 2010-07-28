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
	GameInfoQuery( ThreadFactory* pFactory, servAddr sAddr );

	void				EntryPoint( void );
	GameserverInfo*		GetGSInfo() { return m_pGSInfo; }
	void				QueryforASINFO( void );
    giQuery_state		GetState( void ) { return m_iState; }
	void				SetParent( GameStats* pStats ) { m_pParent = pStats; }
	const char*   		GetClassName( void ) { return "GameInfoQuery"; }
    virtual void        Log( const char* logMsg );
	bool				IsGameInfoQuery( void ) { return true; }

private:
	giQuery_state	    m_iState;
	GameserverInfo*		m_pGSInfo;
	GameStats*		    m_pParent;

	void				ParseASINFO( const char*, size_t );
};

#endif // GAMEINFOQUERY_H
