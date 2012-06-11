#include "CTaskForceLogger.h"

using namespace std;

CTaskForceLogger::CTaskForceLogger()
{
	m_pDisplayLogLines = new CDisplayLogLines();
	m_strLogfileName = "out.log";

	pthread_mutex_init(&muLogfile, NULL);

	m_strFilelog.open(m_strLogfileName.c_str(), ios::out | ios::app | ios::binary); // TODO, error handling
}

CTaskForceLogger::~CTaskForceLogger()
{
	m_strFilelog.close();
	delete m_pDisplayLogLines;
}

CDisplayLogLines* CTaskForceLogger::getDuplicateLogEntries()
{
	return m_pDisplayLogLines->clone();
}

void CTaskForceLogger::AddLog(string cname, string fname, string logtext, long timestamp)
{
	time_t tstamp = (time_t)timestamp;
	AddLog(cname, fname, logtext, ctime(&tstamp));
}

void CTaskForceLogger::AddLog(string cname, string fname, string logtext)
{
	time_t tstamp = time(0);
	struct tm* local = localtime(&tstamp);
	char strTime[16];
	snprintf(strTime, 16, "%02i:%02i:%02i", local->tm_hour, local->tm_min, local->tm_sec);
	AddLog(cname, fname, logtext, strTime);
}

void CTaskForceLogger::AddLog(string cname, string fname, string logtext, string time)
{
	m_pDisplayLogLines->lock();
	if(m_pDisplayLogLines->size_non_threadsafe() >= DISPLAYLOG_BACKLOG_MAX) {
		m_pDisplayLogLines->pop_back_non_threadsafe();
	}	
	CDisplayLogLine* pLine = new CDisplayLogLine(cname, fname, logtext, time);
#ifdef USE_NCURSES
	m_pDisplayLogLines->push_front_non_threadsafe(pLine);
#else
	cout << pLine->strTime << " " << pLine->strClassName << "::" << pLine->strFuncName << "() " << " " << pLine->strLogText << endl;
#endif
	m_pDisplayLogLines->unlock();

	pthread_mutex_lock(&muLogfile);
	if(m_strFilelog.is_open())
		m_strFilelog << pLine->strTime << " " << pLine->strClassName << "::" << pLine->strFuncName << "() " << " " << pLine->strLogText << endl;
	pthread_mutex_unlock(&muLogfile);
}