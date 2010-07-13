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
#include "ThreadedRequest.h"

#define TIMEOUT_GAMESTATS 30		// How long do we want to wait for the worker to complete ?

using namespace std;

//static GameserverManager* gGSManager = GameserverManager::getInstance();
static MasterserverManager* gMasterManager = MasterserverManager::getInstance();
//static MasterqueryManager* gMasterqueryManager = MasterqueryManager::getInstance();

SourceStats* SourceStats::gSourceStats = NULL;

SourceStats::SourceStats( void )
{
    SetParentClassName( "SourceStats" );
}

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
	Log("SourceStats::main() Adding Masterservers...");

    gMasterManager->AddServer( "216.207.205.99:27011" );
    gMasterManager->AddServer( "216.207.205.98:27011" );

    Log("SourceStats::main() Requesting GameServer for game dystopia...");
    Log("SourceStats::main() Creating worker for game dystopia...");

	pthread_t tThread;
	MMThreadArgs* pThreadArgs = new MMThreadArgs( this, "dystopia" );
	int ret = pthread_create( &tThread, NULL, SourceStats::ThreadGameStats, pThreadArgs );
}

void* SourceStats::ThreadGameStats( void *arg )
{
    MMThreadArgs* pArgs = (MMThreadArgs*)arg;
	SourceStats* pParent = pArgs->GetParent();
	char* gameName = pArgs->GetGameName();

    GameStats* pGStats = new GameStats( pParent, gameName );
    pGStats->SetParentClassName( "GameStats" );
	pGStats->SetTimeout( TIMEOUT_GAMESTATS );
	pGStats->EntryPoint();
}

void SourceStats::AddGameStats( GameStats* pStats )
{
	// TODO, lock
	 //m_vGameStats.push_back( pStats );
	 // TODO, unlock
}

//void SourceStats::CheckFinishedMasterQueries( void )
//{
//#ifdef DEBUG
//    std::cout << "SourceStats::CheckFinishedResults() Iterating finished results..." << std::endl;
//#endif
//
//    gMasterqueryManager->ResetIterator();
//
//    for ( Masterquery* mQuery = gMasterqueryManager->GetFinishedQuery(); mQuery; mQuery = gMasterqueryManager->GetFinishedQuery() )
//    {
//#ifdef DEBUG
//        std::cout << "SourceStats::CheckFinishedResults() Found a finished Masterquery result!" << std::endl;
//#endif
//        mQuery->ResetIterator();
//        for ( GameserverEntry* mEntry = mQuery->GetNextServer(); mEntry; mEntry = mQuery->GetNextServer() )
//        {
//            gGSManager->AddEntry( mEntry );
//        }
//    }
//#ifdef DEBUG
//    std::cout << "SourceStats::CheckFinishedResults() Sending out AS_INFOs to all known gameservers..." << endl;
//#endif
//    gGSManager->RequestAllServInfo();
//}

void SourceStats::CheckFinishedGamestats( void )
{
    Log("CheckFinishedGamestats() Looking for finished GameStats...");
	vector<ThreadedRequest*>::iterator it = m_vThreads.begin();

	while ( it < m_vThreads.end() )
	{
		ThreadedRequest* pThread = (*it);
        GameStats* pStats = dynamic_cast<GameStats*>(pThread);
        if ( pStats->GetState() == GSSTATE_DONE )
        {
			char logout[128];
			snprintf(logout, 128, "SourceStats::CheckFinishedGamestats() found finished stats for game '%s'", pStats->GetGameName());
			Log(logout);
            HandlefinishedStats( pStats );
        }
		it++;
	}
}

void SourceStats::HandlefinishedStats( GameStats* pStats )
{
    // TODO
}

void SourceStats::Loop( void )
{
    while ( true )
    {
        CheckThreads();				// for stats only, check for finished threads
        //CheckFinishedMasterQueries();		// check for finished results and do a AS_INFO for those
        //CheckFinishedGamestats();		// check for finished as_info results
		Log("SourceStats::Loop()");		
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

extern pthread_mutex_t muLog;

void SourceStats::Log( const char* logMsg )
{
    int curThreads = GetActiveThreadNo();
    if ( curThreads > 0 )
    {
        pthread_mutex_lock (&muLog);
        cout << "[" << time(NULL) << "|TF: " << curThreads << "] " << logMsg << endl;
        pthread_mutex_unlock (&muLog);
    }
}
