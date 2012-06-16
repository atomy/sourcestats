#ifndef C_GAMEQUERY_TASK
#define C_GAMEQUERY_TASK

#include "libtaskforce/CWorker.h"
#include <string.h>
#include "CGameQuery.h"

class CGameQueryTask : public CTask
{
public:
	CGameQueryTask();
	~CGameQueryTask();

	virtual void						run();

	CGameQuery*							getQuery();

private:
	CGameQuery*							m_pQuery;

private:
	void										doStuff();
};

extern IDisplayLogger*		g_pLogger;

#endif // C_GAMEQUERY_TASK