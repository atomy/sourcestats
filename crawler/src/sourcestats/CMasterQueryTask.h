#ifndef C_MASTERQUERY_TASK
#define C_MASTERQUERY_TASK

#include "libtaskforce/CWorker.h"
#include <string.h>
#include "CMasterQuery.h"

class CMasterQueryTask : public CTask
{
public:
	CMasterQueryTask();
	~CMasterQueryTask();

	virtual void						run();

private:
	void										doStuff();

	CMasterQuery*						m_pQuery;
};

extern IDisplayLogger*		g_pLogger;

#endif // C_MASTERQUERY_TASK