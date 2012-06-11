#include "CDefaultTask.h"

CDefaultTask::CDefaultTask() : CTask()
{
}

void CDefaultTask::run()
{
	// callback
  OnTaskStarted();

  // testing
  usleep(1000*1000*1); // 3s

	// callback
  OnTaskCompleted();
}
