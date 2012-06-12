#ifndef C_GAMEQUERY_HANDLER
#define C_GAMEQUERY_HANDLER

#include "libtaskforce/CTask.h"
#include "libcursesfrontend/IDisplayLogger.h"
#include "libtaskforce/ITaskForceEventHandler.h"

class CGameQueryHandler : public ITaskForceEventHandler
{
public:
	CGameQueryHandler();
	~CGameQueryHandler();

	virtual void						OnTaskCompleted(CTask* pTask);
	virtual void						OnTaskStarted(CTask* pTask);
	virtual bool						IsValidEvent(CTask* pTask);
private:
};

extern IDisplayLogger*		g_pLogger;

#endif // C_GAMEQUERY_HANDLER