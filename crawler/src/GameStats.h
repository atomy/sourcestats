#ifndef GAMESTATS_H
#define GAMESTATS_H

#include "const.h"
#include <stdio.h>
#include <vector>
#include <time.h>
#include <string.h>
#include <map>
//#include "Masterquery.h"
#include "ThreadFactory.h"

class Masterquery;

class GameStats : public ThreadedRequest, public ThreadFactory
{
public:
    GameStats( const char* szGameName );
    ~GameStats() { }

	void			CreateMasterqueryWorker( void );
	void			CreateGameInfoWorker( void );
	void			Loop( void );
	void			Exec( void );
	void			SetMasterquery( Masterquery* pQuery );
	void			MasterqueryDoneCallback( void );
	void			GameInfoDoneCallback( void );
	void			ProgressInfoResults( void );
	void			AddInfoQuery( GameInfoQuery* );

	static void*	ThreadMasterQuery( void *arg );
	static void*	ThreadInfoQuery( void *arg );

private:
    char 			m_sGameName[32];
	int				m_pInfoCallbacks;
	int				m_pInfoRunning;
	pthread_t		m_tMasterqueryThread;


    Masterquery*	m_pMasterquery;
    time_t			m_tMasterqueryStartTime;

	std::vector<GameInfoQuery*> m_vGameInfoQueries;

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
