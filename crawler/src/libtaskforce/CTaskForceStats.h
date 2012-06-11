#ifndef CTASKFORCE_STATS
#define CTASKFORCE_STATS

#include <string>
#include "libcursesfrontend/IDisplayStats.h"

enum { THREADS_MAX, THREADS_ASSIGNED, THREADS_CURRENT, THREADS_IDLE, JOBS_WAITING, JOBS_MAX, STATS_MAXENTRIES };

class CTaskForceStats : public IDisplayStats
{
public:
  CTaskForceStats();

	virtual bool						entryExists( int i );
	virtual int							getValue( int i );
	virtual void						changeValueBy( int i, int k );
	virtual const string&		getDesc( int i );
};

#endif // CTASKFORCE_STATS
