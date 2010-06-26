#include "GameserverManager.h"
#include "MasterserverManager.h"
#include "MasterqueryManager.h"
#include "Masterquery.h"
#include <iostream>
#include <vector>
#include "SourceStats.h"
#include "signal.h"
#include "errno.h"
#include "GameStats.h"

static GameserverManager* gGSManager = GameserverManager::getInstance();
static MasterserverManager* gMasterManager = MasterserverManager::getInstance();
static MasterqueryManager* gMasterqueryManager = MasterqueryManager::getInstance();

SourceStats* SourceStats::gSourceStats = NULL;

SourceStats* SourceStats::getInstance( void )
{
    if ( !gSourceStats )
    {
        gSourceStats = new SourceStats();
    }
    return gSourceStats;
}

void SourceStats::Destroy( void )
{
    delete gSourceStats;
    gSourceStats = NULL;
}

void SourceStats::Init( void )
{
#ifdef DEBUG
    std::cout << "SourceStats::main() Adding Masterservers..." << std::endl;
#endif

    gMasterManager->AddServer( "216.207.205.99:27011" );
    gMasterManager->AddServer( "216.207.205.98:27011" );

#ifdef DEBUG
    std::cout << "SourceStats::main() Requesting GameServer for game dystopia..." << std::endl;
    std::cout << "SourceStats::main() Creating worker for game dystopia..." << std::endl;
#endif

	pthread_t tThread;
	MMThreadArgs* pThreadArgs = new MMThreadArgs( this, "dystopia" );
	int ret = pthread_create( &tThread, NULL, SourceStats::ThreadGameStats, pThreadArgs );
	AddThread( tThread );
}

void* SourceStats::ThreadGameStats( void *arg )
{
    MMThreadArgs* pArgs = (MMThreadArgs*)arg;
	SourceStats* pParent = pArgs->GetParent();
	char* gameName = pArgs->GetName();

    GameStats* pGStats = new GameStats( gameName );
	pParent->AddGameStats( pGStats );
	pGStats->Exec();
}

void SourceStats::AddGameStats( GameStats* pStats )
{
	 m_vGameStats.push_back( pStats );
}

void SourceStats::CheckFinishedMasterQueries( void )
{
#ifdef DEBUG
    std::cout << "SourceStats::CheckFinishedResults() Iterating finished results..." << std::endl;
#endif

    gMasterqueryManager->ResetIterator();

    for ( Masterquery* mQuery = gMasterqueryManager->GetFinishedQuery(); mQuery; mQuery = gMasterqueryManager->GetFinishedQuery() )
    {
#ifdef DEBUG
        std::cout << "SourceStats::CheckFinishedResults() Found a finished Masterquery result!" << std::endl;
#endif
        mQuery->ResetIterator();
        for ( GameserverEntry* mEntry = mQuery->GetNextServer(); mEntry; mEntry = mQuery->GetNextServer() )
        {
            gGSManager->AddEntry( mEntry );
        }
    }
#ifdef DEBUG
    std::cout << "SourceStats::CheckFinishedResults() Sending out AS_INFOs to all known gameservers..." << endl;
#endif
    gGSManager->RequestAllServInfo();
}

void SourceStats::CheckFinishedGamestats( void )
{
#ifdef DEBUG
    std::cout << "CheckFinishedGamestats() Looking for finished GameInfoRequests..." << endl;
#endif

    gGSManager->ResetIterator();
    for ( GameInfoRequest* pGIRequest = gGSManager->GetFinishedQuery(); pGIRequest; pGIRequest = gGSManager->GetFinishedQuery() )
    {
        GameserverInfo* pGSInfo = pGIRequest->GetGSInfo();
#ifdef DEBUG
        printf( "CheckFinishedGamestats() Got Result! servername: %s -- %d/%d\n", pGSInfo->m_sServername.c_str(), pGSInfo->m_cPlayercount, pGSInfo->m_cMaxplayers );
#endif
    }
}

void SourceStats::Loop( void )
{
    while ( true )
    {
        CheckThreads();				// for stats only, check for finished threads
        CheckFinishedMasterQueries();		// check for finished results and do a AS_INFO for those
        CheckFinishedGamestats();		// check for finished as_info results
#ifdef DEBUG
        std::cout << "SourceStats::Loop() active threads: " << m_vThreads.size() << std::endl;
#endif
        sleep( 2 );
    }
}

//void SourceStats::CheckThreads( void )
//{
//    if ( m_vThreads.size() <= 0 )
//        return;
//
//#ifdef DEBUG
//    std::cout << "SourceStats::CheckThreads() checking '" << m_vThreads.size() << "' threads for completion" << std::endl;
//#endif
//
//    vector <pthread_t>::iterator pIT;
//    pIT = m_vThreads.begin();
//
//    while ( pIT != m_vThreads.end() )
//    {
//        int ret = pthread_kill((*pIT), 0);
//        // threadid not found! erase it from our list
//        if ( ret == ESRCH )
//        {
//            pIT = m_vThreads.erase(pIT);
//#ifdef DEBUG
//            std::cout << "SourceStats::CheckThreads() deleted non-existant thread '" << (*pIT) << "'" << std::endl;
//#endif
//        }
//        else if ( ret != 0 )
//        {
//#ifdef DEBUG
//            std::cout << "SourceStats::CheckThreads() pthread_kill() failed on '" << (*pIT) << "'" << std::endl;
//#endif
//        }
//        else
//        {
//#ifdef DEBUG
//            std::cout << "SourceStats::CheckThreads() thread '" << (*pIT) << "' still running" << std::endl;
//#endif
//            pIT++;
//        }
//    }
//}
