#include "CDisplayLogger.h"
#include <stdio.h>
#include "config.h"
#include <iostream>

using namespace std;

// TODO, stream like class for logging (like cout)
// TODO #define code?
CDisplayLogger::CDisplayLogger()
{
	m_pDisplayLogLines = new CDisplayLogLines();
}

CDisplayLogger::~CDisplayLogger()
{
	delete m_pDisplayLogLines;
}

void CDisplayLogger::AddLog(string cname, string fname, string logtext, long timestamp)
{
  time_t tstamp = (time_t)timestamp;
  AddLog(cname, fname, logtext, ctime(&tstamp));
}

void CDisplayLogger::AddLog(string cname, string fname, string logtext)
{
  time_t tstamp = time(0);
  struct tm* local = localtime(&tstamp);
  char strTime[16];
  snprintf(strTime, 16, "%02i:%02i:%02i", local->tm_hour, local->tm_min, local->tm_sec);
  AddLog(cname, fname, logtext, strTime);
}

// TODO, can we put this code into a #define ?
void CDisplayLogger::AddLog(string cname, string fname, string logtext, string time)
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
}

CDisplayLogLines* CDisplayLogger::getDuplicateLogEntries()
{
	return m_pDisplayLogLines->clone();
}