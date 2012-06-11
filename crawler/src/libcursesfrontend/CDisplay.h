#ifndef CDISPLAY_H
#define CDISPLAY_H

#include "libtaskforce/CTaskForceStats.h"
#include "CCursesFrontend.h"
#include "curses.h"

#define DEF_WINS 10

class CCursesFrontend;

class CDisplay
{
public:
  CDisplay(CCursesFrontend*);
  ~CDisplay();

  void												init();									// set us up
  void												render(long delta);			// draw
  void												update(long delta);			// simulate
  void												resize(void);						// update resize according terminal

	IDisplayStats*							getStats();							// function forwarding stats retrieval

private:
  WINDOW*											m_pWins[DEF_WINS];			// array holding window pointers
  char*												m_szTimestring;					// time string to render
	CCursesFrontend*						m_pFrontend;						// backlink to our boss

  long												m_iRendertime;					// next render in x usec
  long												m_iUpdatetime;					// next update in x usec

	int													m_iMaxLogEntries;				// dynamic log entry cap to render
	int													m_iMaxLogLen;						// dynamic log entry len cap to render
	int													m_iSubWinStartX;				// dynamic subwin position
	int													m_iSubWinStartY;				// dynamic subwin position
	int													m_iSubWinSizeX;					// dynamic subwin position
	int													m_iSubWinSizeY;					// dynamic subwin position

public:
};

#endif // CDISPLAY_H
