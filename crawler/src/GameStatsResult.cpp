#include "GameStatsResult.h"
#include "GameStats.h"
#include <iostream>
#include "GameserverInfo.h"

using namespace std;

GameStatsResult::GameStatsResult( GameStats *pStats ) : m_pGameStats( pStats )
{
    m_iPlayers = 0;
    m_iBots = 0;
    m_iMaxplayers = 0;
    m_iLinux = 0;
    m_iWindows = 0;
    m_iDedicated = 0;
    m_iListen = 0;
    m_iStv = 0;
    m_iSecure = 0;
    m_iInsecure = 0;
    m_iPassworded = 0;
    m_iNotPassworded = 0;
    m_iServers = 0;
}

void GameStatsResult::ProcessGameStats( void )
{
    if ( !m_pGameStats )
        return;

    m_pGameStats->ResetIterator();

    for(GameserverInfo* pInfo = m_pGameStats->GetNextServer(); pInfo; pInfo = m_pGameStats->GetNextServer())
    {
        m_iServers++;

        if(pInfo->m_cPlayercount <= 255)
            m_iPlayers += pInfo->m_cPlayercount;

        if(pInfo->m_cBotcount <= 255)
            m_iBots += pInfo->m_cBotcount;

        if(pInfo->m_cMaxplayers <= 255)
            m_iMaxplayers += pInfo->m_cMaxplayers;

        if(pInfo->m_cOS == 'l')
            m_iLinux++;
        else if(pInfo->m_cOS == 'w')
            m_iWindows++;

        if(pInfo->m_cDedicated == 'l')
            m_iListen++;
        else if(pInfo->m_cDedicated == 'd')
            m_iDedicated++;
        else if(pInfo->m_cDedicated == 'p')
            m_iStv++;

        if(pInfo->m_cISSecure == 1)
            m_iSecure++;
        else
            m_iInsecure++;

        if(pInfo->m_cISPassworded == 1)
            m_iPassworded++;
        else
            m_iNotPassworded++;
    }
}

void GameStatsResult::PrintSummary( void )
{
    std::cout << "==============================================================================" << std::endl;
    std::cout << "================================= RESULT =====================================" << std::endl;
    std::cout << " Stats for game '" << m_pGameStats->GetGameName() << "':" << std::endl;
    std::cout << "=== PLAYER STATISTICS ========================================================" << std::endl;
    std::cout << "players(+bots): " << m_iPlayers << " bots: " << m_iBots << " maxplayers: " << m_iMaxplayers << std::endl << std::endl;
    std::cout << "=== SERVER STATISTICS ========================================================" << std::endl;
    std::cout << "servers: " << m_iServers << " linux: " << m_iLinux << " windows: " << m_iWindows << std::endl;
    std::cout << "dedicated: " << m_iDedicated << " listen: " << m_iListen << " stv: " << m_iStv << std::endl;
    std::cout << "secure: " << m_iSecure << " insecure: " << m_iInsecure << " passworded: " << m_iPassworded << " unpassworded: " << m_iNotPassworded << std::endl << std::endl;
    std::cout << "==============================================================================" << std::endl;
    std::cout << "==============================================================================" << std::endl;
}
