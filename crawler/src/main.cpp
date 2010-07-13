#include <iostream>
#include "SourceStats.h"

using namespace std;

static SourceStats* gSourceStats = SourceStats::getInstance();

pthread_mutex_t muLog;

int main()
{
    pthread_mutex_init(&muLog, NULL);

	gSourceStats->Init();
	gSourceStats->Loop();
}
