#ifndef DISPLAY_LOGLINE_H
#define DISPLAY_LOGLINE_H

#include <string>
#include <stdlib.h>
#include <stdio.h>

#define DISPLAYLOG_MAXLEN 256

using namespace std;

class CDisplayLogLine
{
public:
  CDisplayLogLine(string cname, string fname, string logtext, string time)
	{
		strClassName = cname;
		strFuncName = fname;
		strLogText = logtext;
		strTime = time;
	};
	CDisplayLogLine(const CDisplayLogLine* pLine) {
		strClassName = pLine->strClassName;
		strFuncName = pLine->strFuncName;
		strLogText = pLine->strLogText;
		strTime = pLine->strTime;
	};
  ~CDisplayLogLine() {} // TODO, fill me

	char*		toString()
	{
		// TODO, do this with string
		char* log = (char*)malloc(sizeof(char*) * DISPLAYLOG_MAXLEN); // TODO, lolol mem leak
		snprintf(log, DISPLAYLOG_MAXLEN, "[%s] %s::%s() %s", strTime.c_str(), strClassName.c_str(), strFuncName.c_str(), strLogText.c_str());
		return log;
	};

	CDisplayLogLine* clone() {
		return new CDisplayLogLine(this);
	}


public:
  string	strLogText;
  string	strTime;
  string	strClassName;
  string	strFuncName;
};

#endif // DISPLAY_LOGLINE_H
