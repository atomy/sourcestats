#ifndef CCURSES_FRONTEND
#define CCURSES_FRONTEND

#include "IDisplayStats.h"
#include "IDisplayLogger.h"
#include <pthread.h>
#include "CDisplay.h"
#include <signal.h>

#define UI_VERSION "0.0.1"

class CDisplay;
class CDisplayLogger;

class CCursesFrontend
{
public:
  CCursesFrontend();
	~CCursesFrontend();

	bool										start();
	static void*						init(void* arg);
	void										_init();
	void										run();

	void										setStats(IDisplayStats* pStats);
	IDisplayStats*					getStats();
	IDisplayLogger*					getLogger();
	void										setLogger(IDisplayLogger* pLogger);

	void										OnResize();
	void										update(long mtime);

	static void							handle_sigwinch(int signum);
	void										shutdown();
	pthread_t								getThread();

private:
  IDisplayStats*						m_pStats;
	IDisplayLogger*						m_pLogger;
	CDisplay*									m_pDisplay;
	pthread_mutex_t						muStatsChange;
	pthread_mutex_t						muLoggerChange;
	bool											m_bRun;
	pthread_t									m_iThread;
};

extern CCursesFrontend*			g_pFrontend;

#endif // CCURSES_FRONTEND
