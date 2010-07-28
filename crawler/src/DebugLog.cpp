#include "const.h"
#include <iostream>
#include "DebugLog.h"
#include <time.h>

using namespace std;

extern pthread_mutex_t muLog;

void DebugLog::Log( const char* logMsg )
{
    pthread_mutex_lock (&muLog);
    cout << "[" << time(NULL) << "] " << logMsg << endl;
    pthread_mutex_unlock (&muLog);
}

void DebugLog::LogNoDeadLock( const char* logMsg )
{
    Log( logMsg );
}
