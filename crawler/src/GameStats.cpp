#include <iostream>
#include <pthread.h>
#include "GameStats.h"
#include <string.h>
#include <time.h>
#include "Masterquery.h"
#include "GameInfoQuery.h"
#include "GameserverInfo.h"


#define TIMEOUT_MASTERWORKER 30		// How long do we want to wait for the worker to complete ?
#define TIMEOUT_GAMEINFOWORKER 30	// How long do we want to wait for the game info workers to complete ?
#define RETRY_MASTERWORKER 3		// How often do we retry on unsuccessful result till we give up?
#define RETRY_GAMEINFOWORKER 3		// ^^^

using namespace std;


GameStats::GameStats( const char* szGameName )
{
    strncpy( m_sGameName, szGameName, 32 );
    SetStartTime( time(NULL) );
}

void GameStats::Exec( void )
{
}

void GameStats::CreateMasterqueryWorker( void )
{
	pthread_t tThread;
	MMThreadArgs2* pThreadArgs = new MMThreadArgs2( this, "dystopia" );
	int ret = pthread_create( &tThread, NULL, GameStats::ThreadMasterQuery, pThreadArgs );
}

void* GameStats::ThreadMasterQuery( void *arg )
{
    MMThreadArgs* pArgs = (MMThreadArgs*)arg;
	GameStats* pParent = pArgs->GetParent();
	char* gameName = pArgs->GetName();

	pParent->InitThread( this );
	pParent->AddThread( this );

    Masterquery* pQuery = new Masterquery();
	pQuery->SetTimeout( 30 );							// set thread timeout to 30 seconds
	pParent->SetMasterquery( pQuery );
	pQuery->SetGame( gameName );
	pQuery->Exec();
}

// retrieve stats, and kill it
void GameStats::GameInfoDoneCallback()
{
	m_pInfoCallbacks++;

	// hooray we got all replies so we dont need to wait for any timeouts
	if ( m_pInfoCallbacks == m_pInfoRunning )
		ProgressInfoResults();
}

void GameStats::ProgressInfoResults( void )
{
	vector <GameInfoQuery*>::iterator it;

	for( it = m_vGameInfoQueries.begin(); it < m_vGameInfoQueries.end(); it++ )
	{
		GameInfoQuery* pQuery = (*it);
		if ( pQuery->GetStatus() == STATE_DONE )
		{
			GamserverInfo* gInfo = pQuery->GetGSInfo();
#ifdef DEBUG
			cout << "GameStats::ProgressInfoResults() found completed info result for server '" << gInfo->m_sServername << "'" << endl;
#endif
		}
		else
		{
#ifdef DEBUG
			cout << "GameStats::ProgressInfoResults() found result but wasnt completed in time, skipping..." << endl;
#endif
		}
	}
}

void GameStats::MasterqueryDoneCallback()
{
	CreateGameInfoWorker();
}

void GameStats::SetMasterquery( Masterquery* pQuery )
{
	m_pMasterquery = pQuery;
}

void GameStats::AddInfoQuery( GameInfoQuery* pQuery )
{
	// TODO, lock
	m_vGameInfoQueries.push_back( pQuery );
	// TODO, unlock
}

void GameStats::CreateGameInfoWorker( void )
{
	if ( m_pMasterquery->GetStatus() != STATE_DONE )
	{
		cerr << "CreateGameInfoWorker() tried to create gameinfo workers w/o any valid masterserver response!" << endl;
		return;
	}

	m_pInfoCallbacks = 0;
	m_pInfoRunning = 0;
	m_pMasterquery->ResetIterator();

	for( GameserverEntry* pEntry = m_pMasterquery->GetNextServer(); pEntry; pEntry = m_pMasterquery->GetNextServer(); )
	{
		pthread_t tThread;
		MMThreadArgs* pThreadArgs = new MMThreadArgs( this, pEntry->GetAddr() );
		int ret = pthread_create( &tThread, NULL, GameStats::ThreadMasterQuery, pThreadArgs );
		m_pInfoRunning++;
	}
}

void* GameStats::ThreadInfoQuery( void *arg )
{
    MMThreadArgs2* pArgs = (MMThreadArgs2*)arg;
	GameStats* pParent = pArgs->GetParent();
	servAddr stAddr = pArgs->GetAddr();

	pParent->InitThread( this );
	pParent->AddThread( this );

    GameserverInfoQuery* pQuery = new GameserverInfoQuery( stAddr );
	pQuery->SetTimeout( 30 );							// set thread timeout to 30 seconds
	pParent->AddInfoQuery( pQuery );
	pQuery->Exec();
}

void GameStats::Loop( void )
{
    CheckThreads();
    sleep(1);
}
