#include "CTaskForce.h"
#include "libcursesfrontend/CDisplayLogger.h"
#include "CTaskForceStats.h"
#include <stdlib.h>
#include <pthread.h>
#include <iostream>
#include <string.h>
#include "CTaskForceLogger.h"
#include "CDefaultTask.h"

#define MAIN_LOOP_MS 100

using namespace std;

const int g_iMaxTasks = 5;
const int g_iMaxThreads = 3;

CTaskForce* g_pTaskForce = new CTaskForce(g_iMaxTasks, g_iMaxThreads);
IDisplayStats* g_pStats = new CTaskForceStats();
CTaskForceLogger* g_pLogger = new CTaskForceLogger();

bool bShuttingDown = false;

void cleanup() {
	// clean up
	delete g_pTaskForce;
	delete g_pStats;
	g_pTaskForce = NULL;
	g_pStats = NULL;
}

// try to exit and cleanup, on 2nd call exit now
void stop(int signum) {	
	g_pTaskForce->shutdown();

	if(bShuttingDown) {
		cout << endl << "stop() received '" << strsignal(signum) << "' again, hard shutdown..." << endl;
		exit(0);
	}

	cout << endl << "stop() received '" << strsignal(signum) << "', soft shutdown..." << endl;
	bShuttingDown = true;

	pthread_join(g_pTaskForce->getThread(), NULL);

	cleanup();
	cout << "shutdown completed." << endl;

	exit(0);
}

int main(int argc, char **argv) {
	signal(SIGTERM, stop);
	signal(SIGINT, stop);
	signal(SIGHUP, stop);
	signal(SIGUSR1, stop);
	signal(SIGUSR2, stop);

	// fire up
	if(!g_pTaskForce->start()) {
		cerr << "ERROR: taskforce unavailable, exiting..." << endl;
		exit(1);
	}


	g_pStats->changeValueBy(JOBS_MAX, g_iMaxTasks);
	g_pStats->changeValueBy(THREADS_MAX, g_iMaxThreads);

	g_pTaskForce->setStats(g_pStats);
	g_pTaskForce->setLogger(g_pLogger);

	// create jobs till max
	IDisplayStats* pStats = g_pTaskForce->getStats();
	while(pStats->getValue(JOBS_MAX) > pStats->getValue(JOBS_WAITING)) {
		CTask* pTask = new CDefaultTask();
		if(!g_pTaskForce->AddTask(pTask)) {
			cerr << "ERROR while creating task, exiting..." << endl;
			delete pTask;
			stop(-1);
		}
	}

	// this is a never ending story!
	while(1) {
		usleep(1000*MAIN_LOOP_MS);
	}

	return 0;
}