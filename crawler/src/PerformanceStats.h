#ifndef PERFORMANCESTATS_H
#define PERFORMANCESTATS_H


class PerformanceStats
{
public:
	PerformanceStats();
	~PerformanceStats() { }

private:
    const int   m_iMaxRetries;
    int         m_iCurrentRetries;
};

#endif // PERFORMANCESTATS_H
