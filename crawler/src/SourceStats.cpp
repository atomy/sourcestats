#include "MasterserverManager.h"
#include "Masterquery.h"
#include <iostream>
#include <vector>
#include "SourceStats.h"
#include "signal.h"
#include "errno.h"
#include "GameStats.h"
#include "ThreadedRequest.h"
#include "DBProcessor.h"

#define TIMEOUT_GAMESTATS 120		// How long do we want to wait for the worker to complete ?
#define TIMEOUT_DBPROCESSOR 30		// How long do we want to wait for the worker to complete ?

using namespace std;

static MasterserverManager* gMasterManager = MasterserverManager::getInstance();

extern pthread_mutex_t muLog;
SourceStats* SourceStats::gSourceStats = NULL;

SourceStats::SourceStats( void )
{
    //SetParentClassName( "SourceStats" );
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

    gMasterManager->AddServer( "72.165.61.153:27015" );
    gMasterManager->AddServer( "63.234.149.83:27011" );
    gMasterManager->AddServer( "63.234.149.90:27011" );

	const char* sGameName = "dystopia";
	char log[128];
	snprintf(log, 128, "SourceStats::main() Requesting GameServer for game '%s'...", sGameName);
    Log(log);
    snprintf(log, 128, "SourceStats::main() Creating worker for game '%s'...", sGameName);
	Log(log);

	pthread_t tThread;
	MMThreadArgs* pThreadArgs = new MMThreadArgs( this, sGameName );
	int ret = pthread_create( &tThread, NULL, SourceStats::ThreadGameStats, pThreadArgs );
}

void* SourceStats::ThreadGameStats( void *arg )
{
	MMThreadArgs* pArgs = (MMThreadArgs*)arg;
	SourceStats* pParent = pArgs->GetParent();
	const char* pGameName = pArgs->GetGameName();

	GameStats* pStats = new GameStats( pParent, pGameName );
	pStats->Init();
	pStats->SetTimeout( TIMEOUT_GAMESTATS );
	//pDB->SetParentClassName( "SourceStats" );
	pStats->EntryPoint();
}


void* SourceStats::ThreadDatabase( void *arg )
{
	MMThreadArgs2* pArgs = (MMThreadArgs2*)arg;
	SourceStats* pParent = pArgs->GetParent();
	GameStats* pStats = pArgs->GetGameStats();

	DBProcessor* pDB = new DBProcessor( pParent, pStats );
	pDB->SetTimeout( TIMEOUT_DBPROCESSOR );
	pDB->Init();
	//pDB->SetParentClassName( "SourceStats" );
	pDB->EntryPoint();
}

void SourceStats::AddGameStats( GameStats* pStats )
{
	// TODO, lock
	 //m_vGameStats.push_back( pStats );
	 // TODO, unlock
}

void SourceStats::CheckFinishedGamestats( void )
{
    Log("CheckFinishedGamestats() Looking for finished GameStats...");
	vector<ThreadedRequest*>::iterator it = m_vThreads.begin();

	while ( it < m_vThreads.end() )
	{
		ThreadedRequest* pThread = (*it);
        GameStats* pStats = dynamic_cast<GameStats*>(pThread);
        if ( pStats && pStats->GetState() == GSSTATE_WAITINGFORDB )
        {
			char logout[128];
			snprintf(logout, 128, "SourceStats::CheckFinishedGamestats() found finished stats for game '%s'", pStats->GetGameName());
			Log(logout);
            HandlefinishedStats( pStats );
        }
		it++;
	}
}

void SourceStats::CheckFinishedDBProcessors( void )
{
	Log("CheckFinishedDBProcessors() Looking for finished DBProcessors...");
	vector<ThreadedRequest*>::iterator it = m_vThreads.begin();

	while ( it < m_vThreads.end() )
	{
		ThreadedRequest* pThread = (*it);
		DBProcessor* pDB = dynamic_cast<DBProcessor*>(pThread);
		if ( pDB && pDB->GetState() == DBSTATE_DONE )
		{
			char logout[128];
			const char* gameName = pDB->GetGameName();
			snprintf(logout, 128, "SourceStats::CheckFinishedDBProcessors() found finished db processor for game '%s'", gameName);
			Log(logout);
		}
		it++;
	}
}

void SourceStats::HandlefinishedStats( GameStats* pStats )
{
	pStats->SetState(GSSTATE_PROCDB);
	GameStatsResult* pResult = new GameStatsResult(pStats);
	pResult->ProcessGameStats();
	pResult->PrintSummary();
	delete pResult;
//	pthread_t tThread; // temporary disabled
//	MMThreadArgs2* pThreadArgs = new MMThreadArgs2( this, pStats );
//	int ret = pthread_create( &tThread, NULL, SourceStats::ThreadDatabase, pThreadArgs );
}

void SourceStats::Loop( void )
{
    while (1)
    {
		CheckFinishedGamestats();
		CheckFinishedDBProcessors();
        CheckThreads();				// for stats only, check for finished threads
		Log("SourceStats::Loop()");
        sleep(5);

		if ( GetActiveThreadNo() == 0 )
		{
			Log("SourceStats::Loop() yo all of our kids are dead or we've killed them, lets exit quietly");
			exit(0);
		}
    }
}

void SourceStats::Log( const char* logMsg )
{
    int curThreads = GetActiveThreadNo();
    pthread_mutex_lock (&muLog);
    cout << "[" << time(NULL) << "|TF: " << curThreads << "] " << logMsg << endl;
    pthread_mutex_unlock (&muLog);
}

void SourceStats::LogNoDeadLock( const char* logMsg )
{
	pthread_mutex_lock (&muLog);
	cout << "[" << time(NULL) << "|TF: ???] " << logMsg << endl;
	pthread_mutex_unlock (&muLog);
}
