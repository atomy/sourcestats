#ifndef CDEFAULT_DISPLAYSTATS
#define CDEFAULT_DISPLAYSTATS

#include <string>
#include <pthread.h>
#include "IDisplayStats.h"

using namespace std;

class CDefaultDisplayStats : public IDisplayStats
{
public:
  CDefaultDisplayStats();
	~CDefaultDisplayStats();

	virtual	void							changeValueBy(int i, int k);
	virtual int								getValue(int i);
	virtual const string&			getDesc(int i);
	virtual bool							entryExists( int i );
};

#endif // CDEFAULT_DISPLAYSTATS
