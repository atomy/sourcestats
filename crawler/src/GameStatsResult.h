#ifndef GAMESTATSRESULT_H
#define GAMESTATSRESULT_H

#include <stdio.h>
#include "GameStats.h"


class GameStatsResult
{
public:
    GameStatsResult( GameStats *pStats );
	~GameStatsResult() { }

	void    PrintSummary( void );
    void    ProcessGameStats( void );

private:
	int m_iPlayers;
	int m_iBots;
	int m_iMaxplayers;
	int m_iLinux;
	int m_iWindows;
	int m_iDedicated;
	int m_iListen;
	int m_iStv;
	int m_iSecure;
	int m_iInsecure;
	int m_iPassworded;
	int m_iNotPassworded;
	int m_iServers;

	GameStats*  m_pGameStats;
};

#endif // GAMESTATSRESULT_H

