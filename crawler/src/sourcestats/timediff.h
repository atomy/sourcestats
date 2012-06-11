#ifndef TIMEDIFF_H
#define TIMEDIFF_H

#include <sys/time.h>

class CTimeDiff
{
public:
	CTimeDiff() {
		gettimeofday(&lastrun, NULL);
		gettimeofday(&lastrun2, NULL);
	};

	void setNow() {
		gettimeofday(&lastrun, NULL);
		sec = lastrun.tv_sec - lastrun2.tv_sec;
		usec = lastrun.tv_usec - lastrun2.tv_usec;
		gettimeofday(&lastrun2, NULL);
		mtime = ((sec) * 1000 * 1000 + usec);
	};

	long getDiff() { return mtime; }

private:
	struct timeval lastrun, lastrun2;
	long mtime, sec, usec;
};

#endif // TIMEDIFF_H