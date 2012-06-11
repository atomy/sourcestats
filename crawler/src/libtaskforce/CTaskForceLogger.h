#ifndef CTASKFORCE_LOGGER
#define CTASKFORCE_LOGGER

#include <string>
#include "libcursesfrontend/IDisplayLogger.h"
#include <pthread.h>
#include <iostream>
#include <stdio.h>
#include <fstream>

#define DISPLAYLOG_BACKLOG_MAX 20

using namespace std;

class CTaskForceLogger : public IDisplayLogger
{
public:
	CTaskForceLogger();
	~CTaskForceLogger();

	virtual CDisplayLogLines*						getDuplicateLogEntries();
	virtual void												AddLog(string cname, string fname, string logtext, long time);
	virtual void												AddLog(string cname, string fname, string logtext);

protected:
	virtual void												AddLog(string cname, string fname, string logtext, string time);

private:
	string															m_strLogfileName;
	pthread_mutex_t											muLogfile;
	ofstream														m_strFilelog;
};


#endif // CTASKFORCE_LOGGER