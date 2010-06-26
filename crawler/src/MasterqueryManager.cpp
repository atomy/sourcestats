#include "MasterqueryManager.h"
#include "MasterserverManager.h"
#include <iostream>
#include "SourceStats.h"
#include <pthread.h>

static MasterserverManager* gMasterserverManager = MasterserverManager::getInstance();
static SourceStats* gSourceStats = SourceStats::getInstance();

MasterqueryManager* MasterqueryManager::gMasterqueryManager = NULL;

MasterqueryManager* MasterqueryManager::getInstance( void )
{
    if ( !gMasterqueryManager )
    {
        gMasterqueryManager = new MasterqueryManager();
    }
    return gMasterqueryManager;
}

void MasterqueryManager::Destroy()
{
    delete gMasterqueryManager;
    gMasterqueryManager = NULL;
}

void MasterqueryManager::CreateGameWorker( const char* gameName )
{
    char* pGameName = (char*)malloc( sizeof(char) * 32 );
    strncpy( pGameName, gameName, 32 );
    pthread_t pth;

    MMThreadArgs* pThreadArgs = new MMThreadArgs( this, pGameName );
    int ret = pthread_create( &pth, NULL, MasterqueryManager::ThreadGetserversForGame, pThreadArgs );

    if ( ret != 0 )
    {
#ifdef DEBUG
        std::cout << "MasterqueryManager::CreateGameWorker() error creating thread!" << strerror(errno) << std::endl;
#endif
    }
    else
    {
        gSourceStats->AddThread( pth );
    }
}

// static
void* MasterqueryManager::ThreadGetserversForGame( void *arg )
{
    MMThreadArgs* pArgs = (MMThreadArgs*)arg;
    const char *szGameName = pArgs->GetGame();
    pthread_t pID = pthread_self();
#ifdef DEBUG
    std::cout << "MasterqueryManager::ThreadGetserversForGame() thread '" << pID << "' started for fetching game '" << szGameName << "'" << pID << std::endl;
#endif
    //sleep( 3 );

    Masterquery* pMasterquery = new Masterquery();    
    MasterqueryManager* pThis = pArgs->GetObj();
    // TODO, lock
    pThis->AddQuery( pMasterquery );
    // TODO, unlock

#ifdef DEBUG
    std::cout << "MasterqueryManager::GetserversForGame() getting servers for game: " << szGameName << std::endl;
#endif

    pMasterquery->SetGame( szGameName );
    pMasterquery->SetMaster( gMasterserverManager->GetServerAdress() );
    pMasterquery->Exec();
}

void MasterqueryManager::AddQuery( Masterquery* pQuery )
{
    m_vMasterQueries.push_back(pQuery);
}

Masterquery* MasterqueryManager::GetFinishedQuery( void )
{
    if ( m_vMasterQueries.size() <= 0 )
    {
        std::cerr << "MasterqueryManager::GetFinishedQuery() ERROR, m_vMasterQueries is empty" << endl;
        return 0;
    }

    for ( ; m_mqIt != m_vMasterQueries.end(); m_mqIt++ )
    {
        if ( (*m_mqIt)->GetStatus() == STATE_DONE )
        {
            Masterquery* pQuery = (*m_mqIt);
            m_mqIt++;
            return pQuery;
        }
        else
        {
            std::cout << "MasterqueryManager::GetFinishedQuery() found masterquery but isnt done yet" << std::endl;
            continue;
        }
    }

    return 0;
}

void MasterqueryManager::ResetIterator( void )
{
    m_mqIt = m_vMasterQueries.begin();
}
