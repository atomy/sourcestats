#ifndef C_DBPROCESSOR_H
#define C_DBPROCESSOR_H

#include <stdio.h>
#include <mysql++.h>
#include <pthread.h>
#include "IDisplayLogger.h"

class CDBProcessor
{
public:
	CDBProcessor();
	~CDBProcessor();

	static void*								init(void* arg);

	bool												start();
	void												update(long mtime);

	void												shutdown();
	pthread_t										getThread();

private:
	void												_init();
	void												run();
	void												connectDB();
	bool												isConnected();

private:
	pthread_mutex_t							muDBChange;
	bool												m_bRun;
	pthread_t										m_iThread;
	mysqlpp::Connection*				m_pSQLConn;
	long												m_iNextConnectionTry;

	// next connect try in x milliseconds
	long												m_iNextConnectionTryIn;
};

extern IDisplayLogger* g_pLogger;

#endif // C_DBPROCESSOR_H
