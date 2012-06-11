#include "const.h"
#include "PerformanceStats.h"

using namespace std;

PerformanceStats::PerformanceStats( int iMaxRetries ) : m_iMaxRetries( iMaxRetries )
{
    m_iCurrentRetries = 0;
}
