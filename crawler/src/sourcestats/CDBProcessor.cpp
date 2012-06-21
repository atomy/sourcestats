#include "CDBProcessor.h"
#include "timediff.h"

#define DBPROC_SLEEP_DEFAULT_MS 1000
#define DBPROC_CONNECT_RETRY_TIME_MS 60*1000

#define DB_NAME "test"
#define DB_USER "test"
#define DB_PASS "test"
#define DB_HOST "localhost"

#define CNAME "CDBProcessor"

CDBProcessor::CDBProcessor() 
{
	pthread_mutex_init(&muDBChange, NULL);

	m_iNextConnectionTry = 0;
	m_pSQLConn = NULL;
}

CDBProcessor::~CDBProcessor()
{
	if(m_pSQLConn != NULL) {
			m_pSQLConn->close();
			delete m_pSQLConn;
	}		
}

bool CDBProcessor::start()
{
	int ret = pthread_create( &m_iThread, NULL, &CDBProcessor::init, this );
	if(ret == 0)
		return true;
	return false;
}

// thread entry point
void* CDBProcessor::init(void* arg)
{
	CDBProcessor* pThis = (CDBProcessor*)arg;

	// go into the objects loop
	pThis->_init();
	pThis->run();

	return NULL;
}

void CDBProcessor::run()
{
	m_bRun = true;
	CTimeDiff time;

	while(m_bRun) {
		time.setNow();

		// run
		update(time.getDiff());

		usleep(1000*DBPROC_SLEEP_DEFAULT_MS);
	}
}

void CDBProcessor::update(long mtime)
{
	pthread_mutex_lock(&muDBChange);
	if(isConnected()) {
		g_pLogger->AddLog(CNAME, __FUNCTION__, "database is online!");
	} else {
		char log[255];
		snprintf(log, 255, "database is offline, next try in: '%.1f's", m_iNextConnectionTryIn / 1000.0f);
		g_pLogger->AddLog(CNAME, __FUNCTION__, log);

		if(m_iNextConnectionTryIn <= 0) {
			m_iNextConnectionTryIn = DBPROC_CONNECT_RETRY_TIME_MS;
			connectDB();
		} else {
			m_iNextConnectionTryIn -= (mtime / 1000);
		}
	}
	pthread_mutex_unlock(&muDBChange);
}

void CDBProcessor::_init()
{
}

void CDBProcessor::shutdown()
{
	m_bRun = false;
}

pthread_t CDBProcessor::getThread()
{
	return m_iThread;
}

// not thread safe!
void CDBProcessor::connectDB()
{
	try {
		m_pSQLConn = new mysqlpp::Connection(DB_NAME, DB_HOST, DB_USER, DB_PASS);
	} catch(mysqlpp::Exception& e) {
		char log[255];
		snprintf(log, 255, "exception while trying to connect '%s'", e.what());
		g_pLogger->AddLog(CNAME, __FUNCTION__, log);
		return;
	}
	

	if(m_pSQLConn->connected()) {
		g_pLogger->AddLog(CNAME, __FUNCTION__, "connect successful!");
	}	else {
		char log[255];
		snprintf(log, 255, "connect failed! '%s'",m_pSQLConn->error());
		g_pLogger->AddLog(CNAME, __FUNCTION__, log);
	}
}

// not thread safe!
bool CDBProcessor::isConnected() {
	if(m_pSQLConn != NULL && m_pSQLConn->connected())
		return true;
	return false;
}