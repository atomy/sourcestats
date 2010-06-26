#include "GameserverManager.h"
#include <iostream>
#include "GameserverEntry.h"
#include "GameInfoRequest.h"
#include <vector>
#include "SourceStats.h"

static SourceStats* gSourceStats = SourceStats::getInstance();

GameserverManager* GameserverManager::gGameserverManager = NULL;

GameserverManager* GameserverManager::getInstance( void )
{
    if ( !gGameserverManager )
    {
        gGameserverManager = new GameserverManager();
    }
    return gGameserverManager;
}

void GameserverManager::Destroy()
{
    CleanupServerInfos();
    delete gGameserverManager;
    gGameserverManager = NULL;
}

void GameserverManager::CleanupServerInfos( void )
{
}

/**
 * loop through all servers and request their AS_INFOS
 */
void GameserverManager::RequestAllServInfo( void )
{
    vector<GameserverEntry*>::const_iterator ii;
    if ( m_vServerEntries.size() <= 0 )
        std::cerr << "GameserverManager::RequestAllServInfo() ERROR: m_vServerInfos is empty!" << std::endl;

    for ( ii = m_vServerEntries.begin(); ii != m_vServerEntries.end(); ii++ )
    {
        GameserverEntry* gEntry = *ii;
        RequestServInfo(gEntry->GetAddr());
    }

    ExecPendingInfos();
}

/**
 * request AS_INFO from given server
 */
void GameserverManager::RequestServInfo( servAddr sAddr )
{
    GameInfoRequest* pGIRequest = new GameInfoRequest( sAddr );
    m_vInfoRequests.push_back( pGIRequest );
}

void GameserverManager::AddEntry( GameserverEntry* pEntry )
{
    if ( !pEntry )
        std::cerr << "GameserverManager::AddEntry() tried to add null obj!" << std::endl;

    char output[128];
    servAddr2String( output, 128, pEntry->GetAddr() );
    std::cout << "GameserverManager::AddEntry() added new entry with address: " << output 
      << " size: " << m_vServerEntries.size() << endl;

    m_vServerEntries.push_back(pEntry);
}

void GameserverManager::ExecPendingInfos( void )
{
    vector<GameInfoRequest*>::iterator it_giRequest;

    for (it_giRequest = m_vInfoRequests.begin();
            it_giRequest != m_vInfoRequests.end();
            it_giRequest++)
    {
        GameInfoRequest* pGIRequest = (*it_giRequest);
	CreateASWorker( pGIRequest );        
    }
}

void GameserverManager::CreateASWorker( GameInfoRequest* pRequest )
{
    pthread_t pth;

    MMThreadArgs* pThreadArgs = new MMThreadArgs( this, pRequest );
    int ret = pthread_create( &pth, NULL, GameserverManager::ThreadGetASInfo, pThreadArgs );

    if ( ret != 0 )
    {
#ifdef DEBUG
        std::cout << "GameserverManager::CreateASWorker() error creating thread: " << strerror(errno) << std::endl;
#endif
    }
    else
    {
        gSourceStats->AddThread( pth );
    }
}

// static
void* GameserverManager::ThreadGetASInfo( void *arg )
{
    MMThreadArgs* pArgs = (MMThreadArgs*)arg;
    GameInfoRequest* pRequest = pArgs->GetRequest();
    pRequest->QueryforASINFO();
}

GameInfoRequest* GameserverManager::GetFinishedQuery( void )
{
    if ( m_vInfoRequests.size() <= 0 )
    {
        std::cerr << "GameserverManager::GetFinishedQuery() ERROR, m_vMasterQueries is empty" << endl;
        return 0;
    }

    for ( ; m_itGIReq != m_vInfoRequests.end(); m_itGIReq++ )
    {
        if ( (*m_itGIReq)->GetStatus() == STATE_DONE )
        {
            GameInfoRequest* pGIRequest = (*m_itGIReq);
            m_itGIReq++;
            return pGIRequest;
        }
        else
        {
            std::cout << "GameserverManager::GetFinishedQuery() found gameinforequest but isnt done yet" << std::endl;
            continue;
        }
    }

    return 0;
}

void GameserverManager::ResetIterator( void )
{
    m_itGIReq = m_vInfoRequests.begin();
}

int GameserverManager::GetServerCount( void )
{
	return m_vInfoRequests.size();
}
