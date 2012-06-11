#include "CCursesFrontend.h"
#include "CDisplayLogger.h"
#include "CDefaultDisplayStats.h"
#include <stdlib.h>
#include <pthread.h>
#include <iostream>
#include <string.h>

#define MAIN_LOOP_MS 100

using namespace std;

// setup a logger and a stats object
CDefaultDisplayStats* g_pStats = new CDefaultDisplayStats();
CDisplayLogger* g_pLogger = new CDisplayLogger();

CCursesFrontend* g_pFrontend = new CCursesFrontend();

bool bShuttingDown = false;

void cleanup() {
	// clean up
	delete g_pStats;
	delete g_pLogger;	
	delete g_pFrontend;
	g_pStats = NULL;
	g_pLogger = NULL;
	g_pFrontend = NULL;
}

// try to exit and cleanup, on 2nd call exit now
void stop(int signum) {
	g_pFrontend->shutdown();

	if(bShuttingDown) {
		cout << endl << "stop() received '" << strsignal(signum) << "' again, hard shutdown..." << endl;
		exit(0);
	}

	cout << endl << "stop() received '" << strsignal(signum) << "', soft shutdown..." << endl;
	bShuttingDown = true;

	pthread_join(g_pFrontend->getThread(), NULL);

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
	
	// set our objects
	g_pFrontend->setStats(g_pStats);
	g_pFrontend->setLogger(g_pLogger);

	// fire up
	if(!g_pFrontend->start()) {
		cerr << "ERROR: frontend unavailable, exiting..." << endl;
		exit(1);
	}

	// add some example log lines
	g_pLogger->AddLog("testclass", __FUNCTION__, "lal ala la la lala");
	g_pLogger->AddLog("testclass", __FUNCTION__, "lal le a la lala");
	g_pLogger->AddLog("testclass", __FUNCTION__, "lal alete la la lala");

	// this is a never ending story!
	while(1) {
		usleep(1000*MAIN_LOOP_MS);		
	}

  return 0;
}