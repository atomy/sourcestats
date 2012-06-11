#include "CDisplay.h"
#include <string.h>
#include "config.h"
#include <sys/ioctl.h>
#include <iostream>

using namespace std;

CDisplay::CDisplay(CCursesFrontend *pFrontend) : m_pFrontend(pFrontend)
{
	for(int i = 0; i < DEF_WINS; i++) {
		m_pWins[i] = NULL;
	}
  m_szTimestring = (char*)malloc(sizeof(char)*TM_SIZE);
  m_iRendertime = m_iUpdatetime = 0;	
}

CDisplay::~CDisplay()
{
  delwin(m_pWins[1]);
  delwin(m_pWins[0]);

  free(m_szTimestring);

  endwin();
}

void CDisplay::init()
{	
  initscr();
  start_color();
  init_pair(1, COLOR_WHITE, COLOR_BLUE);
  init_pair(2, COLOR_YELLOW, COLOR_BLACK);

  resize();
}

void CDisplay::update(long delta)
{
  if(m_iUpdatetime > 0 && 0) {
    m_iUpdatetime -= delta;
    return;
  }
  m_iUpdatetime = DELTA_UPDATETIME;
  time_t curtime;
  time(&curtime);
  const char* tmstring = ctime(&curtime);
  strncpy(m_szTimestring, tmstring, TM_SIZE);
}

void CDisplay::render(long delta)
{
  if(m_iRendertime > 0) {
    m_iRendertime -= delta;
    return;
  }

	// TODO, null pointer, wtf
	IDisplayStats* pStats = m_pFrontend->getStats();

	wclear(m_pWins[0]);
	wclear(m_pWins[1]);

	wprintw(m_pWins[0], "main window");
	wprintw(m_pWins[1], "logging window");

  m_iRendertime = DELTA_RENDERTIME;

	// date & time display
	wmove(m_pWins[0], 2, 1);		
	wprintw(m_pWins[0], "time: %s", m_szTimestring);

	if(pStats != NULL) {
		int i = 0;
		// 1 digit space as border
		for(int x = 1; x <= 30; x += 29) {
			// 1 line space between subwin and us
			for(int y = 4; y < m_iSubWinStartY - 1; y++) {
				if(!pStats->entryExists(i))
					break;
				wmove(m_pWins[0], y, x);
				wprintw(m_pWins[0], "%s: %d", pStats->getDesc(i).c_str(), pStats->getValue(i));
				i++;
			}
		}

		/*
		// 1st column
		wmove(wins[0], 4, 1);
		wprintw(wins[0], "%s: %d", pStats->getDesc(THREADS_ASSIGNED).c_str(), pStats->getValue(THREADS_ASSIGNED));
		wmove(wins[0], 5, 1);
		wprintw(wins[0], "%s: %d", pStats->getDesc(THREADS_MAX).c_str(), pStats->getValue(THREADS_MAX));
		wmove(wins[0], 6, 1);
		wprintw(wins[0], "%s: %d", pStats->getDesc(THREADS_CURRENT).c_str(), pStats->getValue(THREADS_CURRENT));

		// 2nd column
		wmove(wins[0], 4, 1);
		wprintw(wins[0], "%s: %d", pStats->getDesc(THREADS_IDLE).c_str(), pStats->getValue(THREADS_IDLE));
		wmove(wins[0], 5, 30);
		wprintw(wins[0], "%s: %d", pStats->getDesc(JOBS_WAITING).c_str(), pStats->getValue(JOBS_WAITING));
		wmove(wins[0], 6, 30);
		wprintw(wins[0], "%s: %d", pStats->getDesc(JOBS_MAX).c_str(), pStats->getValue(JOBS_MAX));
		*/
	}

	IDisplayLogger* pLogger = m_pFrontend->getLogger();
	if(pLogger) {
		wmove(m_pWins[1], 0, 0);

		CDisplayLogLines* pLines = pLogger->getDuplicateLogEntries();
		deque<CDisplayLogLine*> pLineContainer = pLines->getLinesNonThreadSave();
		deque<CDisplayLogLine*>::iterator it = pLineContainer.begin();

		int y = 1;
		int x = 1;
		int i = 0;

		while(it < pLineContainer.end()) {
			if(i == m_iMaxLogEntries)
	      break;
			CDisplayLogLine * line = (*it);
			wmove(m_pWins[1], y, x);
			int maxlen = (m_iMaxLogLen > DISPLAYLOG_MAXLEN ? DISPLAYLOG_MAXLEN : m_iMaxLogLen);
			char log[DISPLAYLOG_MAXLEN];
			snprintf(log, maxlen, "[%s] %s::%s() %s", line->strTime.c_str(), line->strClassName.c_str(), line->strFuncName.c_str(), line->strLogText.c_str());
			wprintw(m_pWins[1], log);
			it++;
			y++;
	    i++;
		}
		delete pLines;
	}
	wmove(m_pWins[1], 0, 0);
  wrefresh(m_pWins[0]);
  wrefresh(m_pWins[1]);
}

void CDisplay::resize() 
{
  if(m_pWins[0])
    wclear(m_pWins[0]);
  if(m_pWins[1])
    wclear(m_pWins[1]);

  struct winsize sz;
  ioctl(0, TIOCGWINSZ, &sz);

  m_iSubWinSizeX = (int)((sz.ws_col - (DISPLAY_LOGWIN_BORDER_X * 2))* DISPLAY_LOGWIN_SIZE_X_PCT);
  m_iSubWinSizeY = (int)(sz.ws_row * DISPLAY_LOGWIN_SIZE_Y_PCT);
  m_iSubWinStartX = (int)((sz.ws_col - m_iSubWinSizeX) / 2);
  m_iSubWinStartY = (int)((sz.ws_row - m_iSubWinSizeY) - DISPLAY_LOGWIN_BORDER_BOT);

//  char blubb[128];
//  snprintf(blubb, 128, "display size x: %d y: %d logwin size x: %d y: %d start x: %d y: %d\n", sz.ws_col, sz.ws_row, sizeX, sizeY, startX, startY);
//  cout << blubb << endl;
/*
  char log[64];
  snprintf(log, 64, " display sizeX: %d sizeY: %d", sz.ws_col, sz.ws_row, startX, startY);
  g_displayLogger.AddLog("CDisplay", __FUNCTION__, log);

  snprintf(log, 64, " sizeX: %d sizeY: %d", sizeX, sizeY);
  g_displayLogger.AddLog("CDisplay", __FUNCTION__, log);

  snprintf(log, 64, "startX: %d startY: %d", startX, startY);
  g_displayLogger.AddLog("CDisplay", __FUNCTION__, log);
*/
  resizeterm(sz.ws_row, sz.ws_col);

  if(m_pWins[0] == NULL) {
    m_pWins[0] = newwin(DISPLAY_SIZE_Y, DISPLAY_SIZE_X, DISPLAY_START_Y, DISPLAY_START_X);
  } else {
    wresize(m_pWins[0], DISPLAY_SIZE_Y, DISPLAY_SIZE_X);
    wmove(m_pWins[0], DISPLAY_START_Y, DISPLAY_START_X);
  }

  m_pWins[1] = subwin(m_pWins[0], m_iSubWinSizeY, m_iSubWinSizeX, m_iSubWinStartY, m_iSubWinStartX);
  touchwin(m_pWins[1]);
  
  m_iMaxLogEntries = m_iSubWinSizeY-1;
  m_iMaxLogLen = m_iSubWinSizeX;

  wbkgd(m_pWins[0], COLOR_PAIR(1));
  wbkgd(m_pWins[1], COLOR_PAIR(2));

  bkgd(COLOR_PAIR(1));
  noecho();
}

IDisplayStats* CDisplay::getStats()
{
	return m_pFrontend->getStats();
}