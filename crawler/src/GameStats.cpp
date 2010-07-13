#include <iostream>
#include <pthread.h>
#include "GameStats.h"
#include <string.h>
#include <time.h>
#include "Masterquery.h"
#include "GameInfoQuery.h"
#include "GameserverInfo.h"
#include "MasterserverManager.h"

#define TIMEOUT_MASTERWORKER 20		// How long do we want to wait for the worker to complete ?
#define TIMEOUT_GAMEINFOWORKER 10	// How long do we want to wait for the game info workers to complete ?
#define RETRY_MASTERWORKER 3		// How often do we retry on unsuccessful result till we give up?
#define RETRY_GAMEINFOWORKER 3		// ^^^

using namespace std;
extern pthread_mutex_t muLog;

static MasterserverManager* gMasterManager = MasterserverManager::getInstance();

GameStats::GameStats( ThreadFactory* pFactory, const char* szGameName ) : ThreadedRequest( pFactory )
{
    strncpy( m_sGameName, szGameName, 32 );
    SetStartTime( time(NULL) );
	SetParentClassName( "GameStats" );
}

void GameStats::EntryPoint( void )
{
	ThreadedRequest::EntryPoint();
	m_iQueryState = GSSTATE_NEW;
	NextStep( GSSTATE_NEW );
	Loop();
    ThreadExit();
}

void GameStats::CreateMasterqueryWorker( void )
{
	pthread_t tThread;
	MMThreadArgs* pThreadArgs = new MMThreadArgs( this, "dystopia" );
	int ret = pthread_create( &tThread, NULL, GameStats::ThreadMasterQuery, pThreadArgs );
}

void* GameStats::ThreadMasterQuery( void *arg )
{
    MMThreadArgs* pArgs = (MMThreadArgs*)arg;
	GameStats* pParent = pArgs->GetObj();
	char* gameName = pArgs->GetGameName();

    Masterquery* pQuery = new Masterquery( pParent );
    pQuery->EntryPoint();
	//pParent->SetMasterquery( pQuery );
	pQuery->SetTimeout( TIMEOUT_MASTERWORKER );
	pQuery->SetGame( gameName );
	pQuery->SetMaster( gMasterManager->GetServerAdress() );
	pQuery->Exec();
}

// retrieve stats, and kill it
//void GameStats::GameInfoDoneCallback( GameInfoQuery* pQuery )
//{
//	GameserverInfo* pInfo = pQuery->GetGSInfo();
//	servAddr addr = pInfo->GetAddr();
//
//    char strIp[16];
//    char strPort[8];
//
//    servAddr2Ip( strIp, 16, addr );
//    servAddr2Port( strPort, 8, addr );
//
//	m_pInfoCallbacks++;
//
//#ifdef DEBUG
//	std::cout << "[" << time(NULL) << "][THREAD|" << GetParentClassName() << "|" << GetThreadId() << "] GameStats::GameInfoDoneCallback() callback for gsinfo entry '" << strIp << ":" << strPort << "'" << std::endl;
//#endif
//
//	// hooray we got all replies so we dont need to wait for any timeouts
//	if ( m_pInfoCallbacks == m_pInfoRunning )
//		ProgressInfoResults();
//}

void GameStats::ProgressInfoResults( void )
{
    //cout << "[" << time(NULL) << "][THREAD|" << GetParentClassName() << "|" << GetThreadId() << "] GameStats::ProgressInfoResults() completed as_info queries for game '" << m_sGameName << "'" << std::endl;
    char output[128];
    snprintf(output, 128, "[%u][THREAD|%s|%u] GameStats::ProgressInfoResults() completed as_info queries for game \'%s\'", (unsigned int)time(NULL), GetParentClassName(), (unsigned int)GetThreadId(), m_sGameName );
    Log( output );

//	vector <GameInfoQuery*>::iterator it;
//
//	for( it = m_vGameInfoQueries.begin(); it < m_vGameInfoQueries.end(); it++ )
//	{
//		GameInfoQuery* pQuery = (*it);
//		if ( pQuery->GetStatus() == STATE_DONE )
//		{
//			GamserverInfo* gInfo = pQuery->GetGSInfo();
//#ifdef DEBUG
//			cout << "GameStats::ProgressInfoResults() found completed info result for server '" << gInfo->m_sServername << "'" << endl;
//#endif
//		}
//		else
//		{
//#ifdef DEBUG
//			cout << "GameStats::ProgressInfoResults() found result but wasnt completed in time, skipping..." << endl;
//#endif
//		}
//	}
}

//// child finished getting servers, lets spawn workers for retrieve AS_INFO for every server entry
//void GameStats::ExitCallback( ThreadedRequest* pThread )
//{
//#ifdef DEBUG
//    std::cout << "[" << time(NULL) << "][THREAD|" << pThread->GetThreadId() << "] GameStats::ExitCallback() Masterquery called back." << std::endl;
//#endif
//	ThreadedRequest::ExitCallback( pThread );
//}
//
//void GameStats::FinishCallback( ThreadedRequest* pThread )
//{
//#ifdef DEBUG
//    std::cout << "[" << time(NULL) << "][THREAD|" << pThread->GetThreadId() << "] GameStats::FinishCallback() Masterquery called back." << std::endl;
//#endif
//	NextStep( GSTATE_WAITINGMASTERQ );
//}

// args, completed step
void GameStats::NextStep( gsQuery_state step )
{
	if ( m_iQueryState != step )
	{
		std::cerr << "[" << time(NULL) << "][THREAD|" << GetThreadId() << "] GameStats::NextStep() state in next step given doesnt match current one!" << std::endl;
	}

	if ( m_iQueryState == GSSTATE_NEW )
	{
		CreateMasterqueryWorker();
		m_iQueryState = GSSTATE_WAITINGMASTERQ;
	}
	else if ( m_iQueryState == GSSTATE_WAITINGMASTERQ )
	{
		CreateGameInfoWorker();
		m_iQueryState = GSSTATE_WAITINGASINFOWORKERS;
	}
	else
	{
		std::cerr << "[" << time(NULL) << "][THREAD|" << GetThreadId() << "] GameStats::NextStep() this shouldnt happen, dunno about our next step!" << std::endl;
	}
}

//void GameStats::SetMasterquery( Masterquery *pQuery )
//{
//	m_pMasterquery = pQuery;
//}

//bool GameStats::LocateMasterquery( void )
//{
//	std::vector<ThreadedRequest*>::iterator it = m_vThreads.begin();
//
//	while ( it < m_vThreads.end() )
//	{
//		ThreadedRequest* pRequest = (*it);
//		Masterquery* pQuery = dynamic_cast<Masterquery*>(pRequest);
//		if ( pQuery )
//		{
//			m_pMasterquery = pQuery;
//			return true;
//		}
//	    it++;
//	}
//	return false;
//}

void GameStats::AddInfoQuery( GameInfoQuery* pQuery )
{
	// TODO, lock
	m_vGameInfoQueries.push_back( pQuery );
	// TODO, unlock
}

// spawning threads for retrieving AS_INFO for all gameserver entries
void GameStats::CreateGameInfoWorker( void )
{
	if ( !m_pMasterquery || m_pMasterquery->GetState() != MQSTATE_DONE )
	{
		std::cerr << "[" << time(NULL) << "][THREAD|" << GetThreadId() << "] GameStats::CreateGameInfoWorker() tried to create gameinfo workers w/o any valid masterserver response!" << endl;
		return;
	}

	m_pInfoCallbacks = 0;
	m_pInfoRunning = 0;
	m_pMasterquery->ResetIterator();

	for( GameserverEntry* pEntry = m_pMasterquery->GetNextServer(); pEntry; pEntry = m_pMasterquery->GetNextServer() )
	{
		pthread_t tThread;
		MMThreadArgs2* pThreadArgs = new MMThreadArgs2( this, pEntry->GetAddr() );
		int ret = pthread_create( &tThread, NULL, GameStats::ThreadInfoQuery, pThreadArgs );
		m_pInfoRunning++;
	}
}

void* GameStats::ThreadInfoQuery( void *arg )
{
    MMThreadArgs2* pArgs = (MMThreadArgs2*)arg;
	GameStats* pParent = pArgs->GetObj();
	servAddr stAddr = pArgs->GetAddr();

    GameInfoQuery* pQuery = new GameInfoQuery( pParent, stAddr );
	pQuery->SetTimeout( TIMEOUT_GAMEINFOWORKER );
	pQuery->EntryPoint();
}

void GameStats::Loop( void )
{
	while (1)
	{
		CheckFinishedMasterqueries();
		CheckFinishedGameInfoQueries();
		CheckThreads();
		sleep(1);
	}
}

void GameStats::CheckFinishedGameInfoQueries( void )
{
	Log("GameStats::CheckFinishedGameInfoQueries() Looking for finished GameInfoQueries...");
return;
	std::vector<ThreadedRequest*>::iterator it = m_vThreads.begin();

	while ( it < m_vThreads.end() )
	{
		ThreadedRequest* pThread = (*it);
        if ( !pThread )
		{
	        cerr << "[" << m_sGameName << "] GameStats::CheckFinishedGameInfoQueries() found null pointer in list!!! (bad)" << endl;
			it++;
			continue;
		}

        if ( !pThread->IsGameInfoQuery() )
        {
            it++;
            continue;
        }

        GameInfoQuery* pInfoQuery = dynamic_cast<GameInfoQuery*>(pThread);
        if ( pInfoQuery && pInfoQuery->GetState() == GISTATE_DONE )
        {
            char output[128];
			char logout[128];
            GameserverInfo* pInfo = pInfoQuery->GetGSInfo();
            servAddr2String( output, 128, pInfo->GetAddr() );
			snprintf(logout, 128, "GameStats::CheckFinishedGameInfoQueries() found finished GameInfoQuery for server '%s'", output);
            Log(logout);
            HandlefinishedGIQuery( pInfoQuery );
        }
		else
		{
            cerr << "[" << m_sGameName << "] GameStats::CheckFinishedGameInfoQueries() couldnt cast pointer!" << endl;
		}
		it++;
	}
}

void GameStats::HandlefinishedGIQuery( GameInfoQuery* pQuery )
{
	GameserverInfo* pGSInfo = pQuery->GetGSInfo();
	char output[128];
	char logout[128];
	servAddr2String(output, 128, pGSInfo->GetAddr());
	snprintf(logout, 128, "GameStats::HandlefinishedGIQuery() found finished GameInfoQuery for server '%s'", output);
	Log(logout);
}

void GameStats::CheckFinishedMasterqueries( void )
{
    Log("GameStats::CheckFinishedMasterqueries() Looking for finished Masterqueries...");
	vector<ThreadedRequest*>::iterator it = m_vThreads.begin();

	while ( it < m_vThreads.end() )
	{
		ThreadedRequest* pThread = (*it);
		if ( !pThread )
		{
			cerr << "[" << m_sGameName << "] GameStats::CheckFinishedMasterqueries() found null pointer in list!!! (bad)" << endl;
			it++;
			continue;
		}

        Masterquery* pQuery = dynamic_cast<Masterquery*>(pThread);
        if ( pQuery && pQuery->GetState() == MQSTATE_DONE )
        {
            Log("GameStats::CheckFinishedMasterqueries() found finished Masterquery");
            HandlefinishedMasterquery( pQuery );
        }
        else
        {
            cerr << "[" << m_sGameName << "] GameStats::CheckFinishedMasterqueries() couldnt cast pointer!" << endl;
        }

		it++;
	}
}

void GameStats::HandlefinishedMasterquery( Masterquery* pQuery )
{
	Log("GameStats::HandlefinishedMasterquery()");
	m_pMasterquery = pQuery;
    CreateGameInfoWorker();
	m_pMasterquery = NULL;
	pQuery->SetKill(true);
}

void GameStats::Log( const char* logMsg )
{
    pthread_mutex_lock (&muLog);
    cout << "[" << time(NULL) << "][THREAD:" << GetThreadId() << "|P:" << GetParentClassName() << "|G:" << m_sGameName << "] "<< logMsg << endl;
    pthread_mutex_unlock (&muLog);
}
