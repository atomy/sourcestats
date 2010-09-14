#ifndef GAMESTATS_H
#define GAMESTATS_H

#include "const.h"
#include <stdio.h>
#include <vector>
#include <time.h>
#include <string.h>
#include <map>
//#include "Masterquery.h"
#include "ThreadedRequest.h"

class Masterquery;
class GameInfoQuery;
class GameserverInfo;


class GameStats : public ThreadedRequest
{
public:
    GameStats( ThreadFactory* pFactory, const char* szGameName );
	~GameStats() { }

	void			CreateMasterqueryWorker( void );
	void			CreateGameInfoWorker( void );
	const char*     GetGameName( void ) { return m_sGameName; }
	void			Loop( void );
	void			EntryPoint( void );
	//bool			LocateMasterquery( void );
	void			SetMasterquery( Masterquery *pQuery );
	void			GameInfoDoneCallback( GameInfoQuery* pQuery );
	void			ProgressInfoResults( void );
	//void			AddInfoQuery( GameInfoQuery* );
	gsQuery_state   GetState( void ) { return m_iQueryState; }
	void			SetState( gsQuery_state state ) { m_iQueryState = state; }
	void            CheckFinishedGameInfoQueries( void );
	void            CheckFinishedMasterqueries( void );
	void            HandlefinishedGIQuery( GameInfoQuery* pQuery );
    void            HandlefinishedMasterquery( Masterquery* pQuery );
    virtual void    Log( const char* logMsg );
	virtual void	TimeoutThread_Callback( ThreadedRequest* pThread );
	virtual void	ExitThread_Callback( ThreadedRequest* pThread );

	const char*   	GetClassName( void ) { return "GameStats"; }
	void			NextStep( gsQuery_state step );
	void			CheckTermination( void );
	GameserverInfo*	GetNextServer( void );
	void			ResetIterator( void );

	static void*	ThreadMasterQuery( void *arg );
	static void*	ThreadInfoQuery( void *arg );

private:
	void			GameInfoCompleted();

    char 			m_sGameName[32];
	int				m_iInfoRunning;
	int				m_iInfoCompleted;
	pthread_t		m_tMasterqueryThread;
	gsQuery_state	m_iQueryState;

    Masterquery*	m_pMasterquery;
    time_t			m_tMasterqueryStartTime;
	std::vector<GameserverInfo*>	m_vGameInfos;
	std::vector<GameserverInfo*>::iterator m_itGI;

	pthread_mutex_t m_iGamestatsMutex;

	//std::vector<GameInfoQuery*> m_vGameInfoQueries;

    class MMThreadArgs
    {
    public:
        MMThreadArgs( GameStats* pObj, const char* pArg ) : pObject( pObj )
        {
            strncpy( pGameName, pArg, 32 );
        }

        char* GetGameName( void ) {
            return pGameName;
        }

        GameStats* GetObj( void ) {
            return pObject;
        }

    private:
        GameStats* 	pObject;
        char        pGameName[32];
    };

    class MMThreadArgs2
    {
    public:
        MMThreadArgs2( GameStats* pObj, servAddr pArg ) : pObject( pObj ), stAddr( pArg ) { }

        servAddr GetAddr( void ) {
            return stAddr;
        }
        GameStats* GetObj( void ) {
            return pObject;
        }

    private:
        GameStats* 	pObject;
        servAddr 	stAddr;
    };
};

#endif // GAMESTATS_H
