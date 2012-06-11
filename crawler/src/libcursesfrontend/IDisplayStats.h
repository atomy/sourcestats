#ifndef IDISPLAY_STATS
#define IDISPLAY_STATS

#include <string>

using namespace std;

#define DEFAULT_STATSENTRIES 12

class IDisplayStats
{
public:
	virtual	void							changeValueBy(int i, int k) = 0;
	virtual int								getValue(int i) = 0;
	virtual const string&			getDesc(int i) = 0;
	virtual bool							entryExists( int i ) = 0;

protected:
	float											m_iValues[DEFAULT_STATSENTRIES];
	string										m_szNames[DEFAULT_STATSENTRIES];
	pthread_mutex_t						m_pMutex[DEFAULT_STATSENTRIES];
};


#endif // IDISPLAY_STATS