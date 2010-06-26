#ifndef SOURCESTATS_H
#define SOURCESTATS_H

#include "const.h"
#include <stdio.h>
#include "GameserverInfo.h"
#include "MasterqueryManager.h"
#include <vector>
#include "GameStats.h"
#include "ThreadFactory.h"


class SourceStats : public ThreadFactory
{
private:
    SourceStats() { }
    SourceStats( const SourceStats& cc );
    ~SourceStats() { }

    static SourceStats*		gSourceStats;

public:
    static SourceStats*		getInstance();
    static void				Destroy();

	static void*			ThreadGameStats( void *arg );

    void					Init( void );
    void					Loop( void );
    void					CheckFinishedMasterQueries( void );
    void					CheckFinishedGamestats( void );
	void					AddGameStats( GameStats* );

private:
    std::vector <GameStats*>		m_vGameStats;

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

};

#endif // SOURCESTATS_H
