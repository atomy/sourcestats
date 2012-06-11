#ifndef CDEFAULT_TASK_H
#define CDEFAULT_TASK_H

#include "CTask.h"

class CDefaultTask : public CTask
{
public:
  CDefaultTask();
  ~CDefaultTask();

  virtual void run();

private:
};

#endif // CDEFAULT_TASK_H
