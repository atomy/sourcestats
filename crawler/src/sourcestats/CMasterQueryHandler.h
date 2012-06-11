#include "libtaskforce/CTask.h"
#include "libcursesfrontend/IDisplayLogger.h"
#include "libtaskforce/ITaskForceEventHandler.h"

class CMasterQueryHandler : public ITaskForceEventHandler
{
public:
	CMasterQueryHandler();
	~CMasterQueryHandler();

	virtual void				OnTaskCompleted(CTask* pTask);
	virtual void				OnTaskStarted(CTask* pTask);
	virtual bool				IsValidEvent(CTask* pTask);
private:
};

extern IDisplayLogger* g_pLogger;