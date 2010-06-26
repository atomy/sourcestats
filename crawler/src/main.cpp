#include <iostream>
#include "SourceStats.h"

using namespace std;

static SourceStats* gSourceStats = SourceStats::getInstance();

int main()
{
	gSourceStats->Init();
	gSourceStats->Loop();
}
