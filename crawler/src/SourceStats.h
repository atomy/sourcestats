#ifndef SOURCESTATS_H
#define SOURCESTATS_H

#include "const.h"
#include <stdio.h>
#include "GameserverInfo.h"
#include <vector>
#include "GameStats.h"
#include "ThreadFactory.h"
#include "GameStatsResult.h"

class SourceStats : public ThreadFactory
{
private:
    SourceStats();
	SourceStats( const SourceStats& cc ) { };
    ~SourceStats() { }

    static SourceStats*		gSourceStats;

public:
    static SourceStats*		getInstance();
    static void				Destroy();

	static void*			ThreadGameStats( void *arg );
	static void*			ThreadDatabase( void *arg );

    void					Init( void );
    void					Loop( void );
    void                    HandlefinishedStats( GameStats* pStats );
    void					CheckFinishedGamestats( void );
	void					CheckFinishedDBProcessors( void );
	void					AddGameStats( GameStats* );
	void                    Log( const char* logMsg );
	void                    LogNoDeadLock( const char* logMsg );

	//virtual const char*     GetName( void ) { return "SourceStats"; }

private:
    std::vector <GameStats*>		m_vGameStats;
    std::vector <GameStatsResult*>	m_vGameStatsResults;

    class MMThreadArgs
    {
    public:
        MMThreadArgs( SourceStats* pObj, const char* pArg ) : m_pParent( pObj )
		{
			m_pGameName = strdup( pArg );
		}

        SourceStats* GetParent( void )
		{
            return m_pParent;
        }

		char* GetGameName( void )
		{
            return m_pGameName;
        }

    private:
        SourceStats* 		m_pParent;
        char*				m_pGameName;
    };

	class MMThreadArgs2
	{
	public:
		MMThreadArgs2( SourceStats* pObj, GameStats* pArg ) : m_pParent( pObj ), m_pGameStats( pArg )
		{ }

		SourceStats* GetParent( void )
		{
			return m_pParent;
		}

		GameStats* GetGameStats( void )
		{
			return m_pGameStats;
		}

	private:
		SourceStats* 		m_pParent;
		GameStats*			m_pGameStats;
	};
};

#endif // SOURCESTATS_H
