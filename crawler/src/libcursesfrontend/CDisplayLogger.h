#ifndef CDISPLAY_LOGGER_H
#define CDISPLAY_LOGGER_H

#include "CDisplayLogLine.h"
#include <deque>
#include "CDisplay.h"
#include "IDisplayLogger.h"
#include "CDisplayLogLines.h"

class CDisplay;

class CDisplayLogger : public IDisplayLogger
{
public:
  CDisplayLogger();
	~CDisplayLogger();

	void												AddLog(string cname, string fname, string logtext, long timestamp);
	void												AddLog(string cname, string fname, string logtext);

	CDisplayLogLines*						getDuplicateLogEntries();

private:
  void												AddLog(string cname, string fname, string logtext, string time);
};

#endif // CDISPLAY_LOGGER_H
