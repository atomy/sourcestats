#include <iostream>
#include <pthread.h>
#include "GameStats.h"
#include <string.h>
#include <time.h>
#include "Masterquery.h"
#include "GameInfoQuery.h"
#include "GameserverInfo.h"
#include "MasterserverManager.h"
#include <iostream>

#define TIMEOUT_MASTERWORKER 100		// How long do we want to wait for the worker to complete ?
#define TIMEOUT_GAMEINFOWORKER 10	// How long do we want to wait for the game info workers to complete ?
//#define RETRY_MASTERWORKER 3		// How often do we retry on unsuccessful result till we give up?
//#define RETRY_GAMEINFOWORKER 3		// ^^^
#define GINFO_MAXPARALLEL 1000

using namespace std;
extern pthread_mutex_t muLog;

static MasterserverManager* gMasterManager = MasterserverManager::getInstance();


GameStats::GameStats( ThreadFactory* pFactory, const char* szGameName ) : ThreadedRequest( pFactory )
{
    strncpy( m_sGameName, szGameName, 32 );
    SetStartTime( time(NULL) );
	SetMasterquery( NULL );
	m_iInfoCompleted = 0;
	m_iInfoRunning = 0;
}

void GameStats::EntryPoint( void )
{
	ThreadedRequest::PreEntryPoint();
	pthread_mutex_init(&m_iGamestatsMutex, NULL);
	m_iQueryState = GSSTATE_WAITINGMASTERQ;
	//NextStep( GSSTATE_NEW );
	CreateMasterqueryWorker();
	ThreadedRequest::PostEntryPoint();
}

void GameStats::CreateMasterqueryWorker( void )
{
	Log("GameStats::CreateMasterqueryWorker() creating worker for masterquery");
	pthread_t tThread;
	MMThreadArgs* pThreadArgs = new MMThreadArgs( this, m_sGameName );
	int ret = pthread_create( &tThread, NULL, GameStats::ThreadMasterQuery, pThreadArgs );
}

void* GameStats::ThreadMasterQuery( void *arg )
{
    MMThreadArgs* pArgs = (MMThreadArgs*)arg;
	GameStats* pParent = pArgs->GetObj();
	char* gameName = pArgs->GetGameName();

    Masterquery* pQuery = new Masterquery( pParent );
	pParent->SetMasterquery(pQuery);
	pQuery->SetTimeout( TIMEOUT_MASTERWORKER );
	pQuery->Init();
	pQuery->SetGame( gameName );
	pQuery->SetMaster( gMasterManager->GetServerAdress() );
	pQuery->EntryPoint();
}

void GameStats::ProgressInfoResults( void )
{
    //cout << "[" << time(NULL) << "][THREAD|" << GetParentClassName() << "|" << GetThreadId() << "] GameStats::ProgressInfoResults() completed as_info queries for game '" << m_sGameName << "'" << std::endl;
    char output[128];
    snprintf(output, 128, "[%u][THREAD|%u] GameStats::ProgressInfoResults() completed as_info queries for game \'%s\'", (unsigned int)time(NULL), (unsigned int)GetThreadId(), m_sGameName );
    Log( output );
}

// args, completed step
void GameStats::NextStep( gsQuery_state step )
{
	//if ( m_iQueryState != step )
	//{
	//	std::cerr << "[" << time(NULL) << "][THREAD|" << GetThreadId() << "] GameStats::NextStep() state in next step given doesnt match current one!" << std::endl;
	//}

	//if ( m_iQueryState == GSSTATE_NEW )
	//{
	//	CreateMasterqueryWorker();
	//	m_iQueryState = GSSTATE_WAITINGMASTERQ;
	//}
	//else if ( m_iQueryState == GSSTATE_WAITINGMASTERQ )
	//{
	//	CreateGameInfoWorker();
	//	m_iQueryState = GSSTATE_WAITINGASINFOWORKERS;
	//}
	//else
	//{
	//	std::cerr << "[" << time(NULL) << "][THREAD|" << GetThreadId() << "] GameStats::NextStep() this shouldnt happen, dunno about our next step!" << std::endl;
	//}
}

void GameStats::SetMasterquery( Masterquery *pQuery )
{
	m_pMasterquery = pQuery;
}

// spawning threads for retrieving AS_INFO for all gameserver entries
void GameStats::CreateGameInfoWorker( void )
{
	if ( m_iQueryState != GSSTATE_WAITINGASINFOWORKERS )
		return;

	if ( !m_pMasterquery || m_pMasterquery->GetState() != MQSTATE_DONE )
	{
		std::cerr << "[" << time(NULL) << "][THREAD|" << GetThreadId() << "] GameStats::CreateGameInfoWorker() tried to create gameinfo workers w/o any valid masterserver response!" << endl;
		return;
	}

	// busy, retry later
	while ( m_iInfoRunning <= GINFO_MAXPARALLEL )
	{
		GameserverEntry* pEntry = m_pMasterquery->GetNextServer();
		if(!pEntry)
		{
			m_iQueryState = GSSTATE_WAITINGASINFOWORKERSFINISH;
			return;
		}

		pthread_t tThread;
		MMThreadArgs2* pThreadArgs = new MMThreadArgs2( this, pEntry->GetAddr() );
		int ret = pthread_create( &tThread, NULL, GameStats::ThreadInfoQuery, pThreadArgs );
		m_iInfoRunning++;
	}

	if (m_iInfoRunning > GINFO_MAXPARALLEL)
	{
		Log("GameStats::CreateGameInfoWorker() busy -- i'm over limit for GameInfo-Queries!");
		return;
	}
}

void* GameStats::ThreadInfoQuery( void *arg )
{
    MMThreadArgs2* pArgs = (MMThreadArgs2*)arg;
	GameStats* pParent = pArgs->GetObj();
	servAddr stAddr = pArgs->GetAddr();

    GameInfoQuery* pQuery = new GameInfoQuery( pParent, stAddr );
	pQuery->SetTimeout( TIMEOUT_GAMEINFOWORKER );
	pQuery->Init();
	pQuery->EntryPoint();
}

void GameStats::ResetIterator( void )
{
	m_itGI = m_vGameInfos.begin();
}

GameserverInfo* GameStats::GetNextServer( void )
{
    if ( m_itGI < m_vGameInfos.begin() || m_itGI > m_vGameInfos.end() )
        throw "GameStats::GetNextServer() ERROR, iterator out of range! Did you ResetIterator?";

	if ( m_vGameInfos.size() <= 0 )
		Log("GameStats::GetNextServer() error, list is empty!");
    else
    {
		char logout[128];
        snprintf(logout, 128, "GameStats::GetNextServer() vector holds '%d' elements", m_vGameInfos.size());
		Log(logout);
    }

	if ( m_itGI >= m_vGameInfos.end() )
		return NULL;

	GameserverInfo* pInfo = (*m_itGI);
	m_itGI++;

	return pInfo;
}

void GameStats::Loop( void )
{
	while (1)
	{
		CheckFinishedMasterqueries();
		CheckFinishedGameInfoQueries();
		CreateGameInfoWorker();
		CheckThreads();
		CheckTermination();
		sleep(1);
	}
}

// check if we are done, if so, mark us as done
void GameStats::CheckTermination( void )
{
	if (m_iQueryState == GSSTATE_WAITINGASINFOWORKERSFINISH)
	{
		if (m_iInfoRunning == 0)
		{
			Log("GameStats::CheckTermination() all gameinfoqueries are done!");
			m_iQueryState = GSSTATE_WAITINGFORDB;
		}
		else
		{
			char logout[128];
			snprintf(logout, 128, "GameStats::CheckTermination() waiting for '%d' gameinfoqueries to finish", m_iInfoRunning);
			Log(logout);
		}
	}
}

void GameStats::CheckFinishedGameInfoQueries( void )
{
	Log("GameStats::CheckFinishedGameInfoQueries() Looking for finished GameInfoQueries...");
    pthread_mutex_lock(&m_threadMutex);
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
			Log("GameStats::CheckFinishedGameInfoQueries() skipping non-gameinfoquery");
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
			m_vGameInfos.push_back(new GameserverInfo((*pInfoQuery->GetGSInfo())));
			pInfoQuery->SetKill(true);
        }
		else if ( !pInfoQuery )
		{
            cerr << "[" << m_sGameName << "] GameStats::CheckFinishedGameInfoQueries() couldnt cast pointer!" << endl;
		}
		it++;
	}
    pthread_mutex_unlock(&m_threadMutex);
}

void GameStats::CheckFinishedMasterqueries( void )
{
	Log("GameStats::CheckFinishedMasterqueries() Looking for finished Masterqueries...");
    if (m_pMasterquery && m_pMasterquery->GetState() == MQSTATE_DONE && m_iQueryState == GSSTATE_WAITINGMASTERQ)
    {
        Log("GameStats::CheckFinishedMasterqueries() Masterquery is done!");
		m_pMasterquery->ResetIterator();
		m_iQueryState = GSSTATE_WAITINGASINFOWORKERS;
		m_pMasterquery->SetKill(true);
    }
}

void GameStats::Log( const char* logMsg )
{
    pthread_mutex_lock (&muLog);
    cout << "[" << time(NULL) << "][THREAD:" << GetThreadId() << "|G:" << m_sGameName << "] "<< logMsg << endl;
    pthread_mutex_unlock (&muLog);
}

void GameStats::TimeoutThread_Callback( ThreadedRequest* pThread )
{
	char log[128];
	snprintf(log, 128, "GameStats::TimeoutThread_Callback() after running for '%u' seconds", (unsigned int)(time(NULL)-pThread->GetStartTime()));
	LogNoDeadLock(log);
}

void GameStats::GameInfoCompleted()
{
	pthread_mutex_lock (&m_iGamestatsMutex);
	m_iInfoRunning--;
	m_iInfoCompleted++;
	pthread_mutex_unlock (&m_iGamestatsMutex);
}

void GameStats::ExitThread_Callback( ThreadedRequest* pThread )
{
	char log[128];
	snprintf(log, 128, "GameStats::ExitThread_Callback() after running for '%u' seconds", (unsigned int)(time(NULL)-pThread->GetStartTime()));
	LogNoDeadLock(log);
	if(pThread->IsGameInfoQuery())
		GameInfoCompleted();
}
