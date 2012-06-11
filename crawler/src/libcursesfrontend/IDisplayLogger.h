#ifndef IDISPLAY_LOGGER
#define IDISPLAY_LOGGER

#include <string>
#include <deque>
#include "CDisplayLogLines.h"
#include "CDisplayLogLine.h"

using namespace std;

class IDisplayLogger
{
public:
	virtual void													AddLog(string cname, string fname, string logtext, long timestamp) = 0;
	virtual void													AddLog(string cname, string fname, string logtext) = 0;
	virtual CDisplayLogLines*							getDuplicateLogEntries() = 0;

protected:
	virtual void													AddLog(string cname, string fname, string logtext, string time) = 0;

	CDisplayLogLines*											m_pDisplayLogLines;
};


#endif // IDISPLAY_LOGGER