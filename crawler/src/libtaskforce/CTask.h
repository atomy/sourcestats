#ifndef CTASK_H
#define CTASK_H

#include "CTaskPool.h"
#include "CWorker.h"
#include "CTaskForce.h"

class CTaskPool;
class CWorker;

class CTask
{
public:
	CTask();

  virtual void run() = 0;
  virtual void OnTaskAssigned(CWorker* pInfo);
  virtual void OnTaskCompleted();
  virtual void OnTaskStarted();

  CWorker* getWorker() { return m_pWorker; }

private:
  CWorker* m_pWorker;
};

extern CTaskForce* g_pTaskForce;

#endif // CTASK_H
