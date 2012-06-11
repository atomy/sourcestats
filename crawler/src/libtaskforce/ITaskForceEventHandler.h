#ifndef ITaskForce_EventHandler
#define ITaskForce_EventHandler

#include "CTask.h"

class CTask;

class ITaskForceEventHandler
{
public:
	virtual void		OnTaskCompleted(CTask* pTask) = 0;
	virtual void		OnTaskStarted(CTask* pTask) = 0;

	virtual bool		IsValidEvent(CTask* pTask) = 0;
};

#endif // ITaskForce_EventHandler