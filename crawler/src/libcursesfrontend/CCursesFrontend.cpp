#include "CCursesFrontend.h"
#include <pthread.h>
#include <assert.h>
#include <sys/time.h>
#include <unistd.h>
#include "../sourcestats/timediff.h"

#define FRONTEND_SLEEP_DEFAULT_MS 100

CCursesFrontend::CCursesFrontend()
{
  pthread_mutex_init(&muStatsChange, NULL);
	pthread_mutex_init(&muLoggerChange, NULL);
}

CCursesFrontend::~CCursesFrontend()
{
	delete m_pDisplay;
}

// TODO, return value?!
bool CCursesFrontend::start()
{
	int ret = pthread_create( &m_iThread, NULL, &CCursesFrontend::init, this );
	if(ret == 0)
		return true;
	return false;
}

// thread entry point
void* CCursesFrontend::init(void* arg)
{
	CCursesFrontend* pThis = (CCursesFrontend*)arg;

	// go into the objects loop
	pThis->_init();
	pThis->run();

	return NULL;
}

void CCursesFrontend::_init()
{
	// resize handler
	signal(SIGWINCH, CCursesFrontend::handle_sigwinch);

  if(m_pDisplay != NULL)
	  delete m_pDisplay;
  m_pDisplay = new CDisplay(this);

	m_pDisplay->init();
}

void CCursesFrontend::run()
{
	m_bRun = true;
	CTimeDiff time;

	while(m_bRun) {
		time.setNow();

		// run
		update(time.getDiff());
		
		usleep(1000*FRONTEND_SLEEP_DEFAULT_MS);
	}
}

void CCursesFrontend::setStats(IDisplayStats* pStats)
{
  pthread_mutex_lock(&muStatsChange);
  m_pStats = pStats;
  pthread_mutex_unlock(&muStatsChange);
}

void CCursesFrontend::setLogger(IDisplayLogger* pLogger)
{
	pthread_mutex_lock(&muLoggerChange);
	m_pLogger = pLogger;
	pthread_mutex_unlock(&muLoggerChange);
}

void CCursesFrontend::OnResize()
{
  m_pDisplay->resize();
}

void CCursesFrontend::update(long mtime)
{
  m_pDisplay->update(mtime);
	m_pDisplay->render(mtime);
}

IDisplayStats* CCursesFrontend::getStats()
{
  pthread_mutex_lock(&muStatsChange);
	IDisplayStats* pStats = m_pStats;
	pthread_mutex_unlock(&muStatsChange);

	//assert(m_pStats != NULL); // TODO, kinda ugly to debug, is there anything better?

  return pStats;
}

IDisplayLogger* CCursesFrontend::getLogger()
{
	pthread_mutex_lock(&muLoggerChange);
	IDisplayLogger* pLogger = m_pLogger;
	pthread_mutex_unlock(&muLoggerChange);

	return pLogger;
}

void CCursesFrontend::handle_sigwinch(int signum)
{
	g_pFrontend->OnResize();
}

void CCursesFrontend::shutdown()
{
	m_bRun = false;
}

pthread_t CCursesFrontend::getThread()
{
	return m_iThread;
}
