#include <string>
#include <iostream>
#include <sys/time.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <string.h>

#include "config.h"
#include "libtaskforce/CTaskForce.h"
#include "libtaskforce/CTaskForceLogger.h"
#include "libtaskforce/CTaskForceStats.h"
#include "libcursesfrontend/CCursesFrontend.h"
#include "libcursesfrontend/IDisplayStats.h"
#include "libcursesfrontend/IDisplayLogger.h"
#include "libtaskforce/CDefaultTask.h"
#include "CMasterQueryTask.h"
#include "CMasterQueryHandler.h"
#include "CGameQueryTask.h"
#include "CGameQueryHandler.h"
#include "CDBProcessor.h"

#define STATS_VERSION "0.0.1"

#define MAIN_SLEEP_DEFAULT_MS 500
#define CNAME "main"

using namespace std;

const int g_iMaxTasks = 5;
const int g_iMaxThreads = 3;

// setup a logger and a stats object
IDisplayStats* g_pStats = new CTaskForceStats();
IDisplayLogger* g_pLogger = new CTaskForceLogger();

// setup the taskforce
CTaskForce* g_pTaskForce = new CTaskForce(g_iMaxTasks, g_iMaxThreads);

// setup the frontend
CCursesFrontend* g_pFrontend = new CCursesFrontend();

// handler for our MasterQuery tasks
CMasterQueryHandler* g_pMasterHandler = new CMasterQueryHandler();
CGameQueryHandler* g_pGameHandler = new CGameQueryHandler();

// our processor for db stuff
CDBProcessor* g_pDBProcessor = new CDBProcessor();

bool bShuttingDown = false;

//void handleTask(CTask* pTask) {
//	g_pLogger->AddLog(CNAME, __FUNCTION__, "handling completed task!");
//	delete pTask;
//}

void cleanup() {
	delete g_pLogger;
	delete g_pStats;
	delete g_pFrontend;
	delete g_pTaskForce;
	delete g_pMasterHandler;
	delete g_pGameHandler;
	delete g_pDBProcessor;

	g_pLogger = NULL;
	g_pStats = NULL;
	g_pFrontend = NULL;
	g_pTaskForce = NULL;
	g_pDBProcessor = NULL;
}

// try to exit and cleanup, on 2nd call exit now
void stop(int signum) {
	g_pFrontend->shutdown();
	g_pTaskForce->shutdown();
	g_pDBProcessor->shutdown();

	if(bShuttingDown) {
		cout << endl << "stop() received '" << strsignal(signum) << "' again, hard shutdown..." << endl;
		exit(0);
	}

	cout << endl << "stop() received '" << strsignal(signum) << "', soft shutdown..." << endl;
	bShuttingDown = true;

	pthread_join(g_pTaskForce->getThread(), NULL);
	pthread_join(g_pFrontend->getThread(), NULL);
	pthread_join(g_pDBProcessor->getThread(), NULL);
	

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

	// set stats default value
	g_pStats->changeValueBy(JOBS_MAX, g_iMaxTasks);
	g_pStats->changeValueBy(THREADS_MAX, g_iMaxThreads);

	/////////////////////////////////////////////////////////
	///////////////// DBPROCESSOR - START ///////////////////

	g_pDBProcessor->start();

	///////////////// DBPROCESSOR - END /////////////////////
	/////////////////////////////////////////////////////////

	/////////////////////////////////////////////////////////
	///////////////// FRONTEND - START //////////////////////

	// set our objects
	g_pFrontend->setStats(g_pStats);
	g_pFrontend->setLogger(g_pLogger);

	// fire up
	g_pFrontend->start();	

	///////////////// FRONTEND - END ////////////////////////
	/////////////////////////////////////////////////////////

	/////////////////////////////////////////////////////////
	///////////////// TASKFORCE - START /////////////////////

	g_pTaskForce->setStats(g_pStats);
	g_pTaskForce->setLogger(g_pLogger);
	g_pTaskForce->InstallEventHandler(g_pMasterHandler);
	g_pTaskForce->InstallEventHandler(g_pGameHandler);

	IDisplayStats* pStats = g_pTaskForce->getStats();
	if(pStats->getValue(JOBS_MAX) > pStats->getValue(JOBS_WAITING)) {
		bool ret = g_pTaskForce->AddTask(new CGameQueryTask());
		//bool ret = g_pTaskForce->AddTask(new CMasterQueryTask());
		if(!ret) {
			g_pLogger->AddLog(CNAME, __FUNCTION__, "something went wrong during task setup");
		}
	}

	// fire up
	if(!g_pTaskForce->start()) {
		cerr << "ERROR: taskforce unavailable, exiting..." << endl;
		exit(1);
	}

	///////////////// TASKFORCE - END ///////////////////////
	/////////////////////////////////////////////////////////

	// loop
	while(1) {
		//g_pLogger->AddLog(CNAME, __FUNCTION__, "handling tasks...");
		//for(CTask* pTask = g_pTaskForce->getNextCompletedTask(); pTask != NULL; pTask = g_pTaskForce->getNextCompletedTask()) {
		//	g_pLogger->AddLog(CNAME, __FUNCTION__, "found task! gonna handle it...");
		//	handleTask(pTask);
		//}
		usleep(1000*MAIN_SLEEP_DEFAULT_MS);
	}

	return 0;
}