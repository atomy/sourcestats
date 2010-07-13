#ifndef DEBUGLOG_H
#define DEBUGLOG_H


class DebugLog
{
public:
	DebugLog() { }
	~DebugLog() { }

	virtual void Log( const char* logMsg );
};

#endif // DEBUGLOG_H
