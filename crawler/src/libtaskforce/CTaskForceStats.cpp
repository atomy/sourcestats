#include "CTaskForceStats.h"

CTaskForceStats::CTaskForceStats()
{
  for(int i = 0; entryExists(i); i++) {
	  switch(i) {
		  case THREADS_ASSIGNED:
			  m_szNames[i] = "threads_assigned";
			break;
			case THREADS_CURRENT:
			  m_szNames[i] = "threads_current";
			break;
			case THREADS_IDLE:
			  m_szNames[i] = "threads_idle";
			break;
			case JOBS_WAITING:
			  m_szNames[i] = "jobs_waiting";
			break;
			case JOBS_MAX:
			  m_szNames[i] = "jobs_max";
			break;
			case THREADS_MAX:
			  m_szNames[i] = "threads_max";
			break;
			default:
			  m_szNames[i].clear();
			break;
		}
	}
}

bool CTaskForceStats::entryExists( int i )
{
	if(i < STATS_MAXENTRIES)
		return true;
	return false;
}

int CTaskForceStats::getValue( int i )
{
	pthread_mutex_lock(&m_pMutex[i]);
	int k = m_iValues[i];
	pthread_mutex_unlock(&m_pMutex[i]);
	return k;
}

void CTaskForceStats::changeValueBy( int i, int k )
{
	pthread_mutex_lock(&m_pMutex[i]);
	m_iValues[i] += k;
	pthread_mutex_unlock(&m_pMutex[i]);
}

const string& CTaskForceStats::getDesc( int i )
{
	return m_szNames[i];
}