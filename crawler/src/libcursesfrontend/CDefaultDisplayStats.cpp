#include "CDefaultDisplayStats.h"
#include <pthread.h>
#include <string>
#include <sstream>

using namespace std;

CDefaultDisplayStats::CDefaultDisplayStats()
{
  for(int i = 0; entryExists(i); i++) {
		ostringstream stream;
		stream << "default";
		stream << i;
	  m_iValues[i] = 0;
		m_szNames[i] = stream.str();
		pthread_mutex_init(&m_pMutex[i], NULL);
	}
}

CDefaultDisplayStats::~CDefaultDisplayStats()
{
}

void CDefaultDisplayStats::changeValueBy(int i, int k)
{
  pthread_mutex_lock(&m_pMutex[i]);
  m_iValues[i] += k;
	pthread_mutex_unlock(&m_pMutex[i]);
}

int CDefaultDisplayStats::getValue(int i)
{
	pthread_mutex_lock(&m_pMutex[i]);
	int k = m_iValues[i];
	pthread_mutex_unlock(&m_pMutex[i]);
	return k;
}

const string& CDefaultDisplayStats::getDesc(int i)
{
	return m_szNames[i];
}

bool CDefaultDisplayStats::entryExists( int i )
{
	if(i < DEFAULT_STATSENTRIES)
		return true;
	return false;
}
