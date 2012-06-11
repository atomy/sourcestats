#ifndef DBPROCESSOR_H
#define DBPROCESSOR_H

#include "const.h"
#include <stdio.h>
#include <mysql++.h>
#include "ThreadedRequest.h"
#include "GameStats.h"


class DBProcessor : public ThreadedRequest
{
public:
	DBProcessor(ThreadFactory*, GameStats*);
	~DBProcessor();

	//virtual void			Init( GameStats* pStats );
	virtual void			Loop( void );
	virtual void            Log( const char* logMsg );
	virtual void            LogNoDeadLock( const char* logMsg );
	virtual const char*		GetClassName() { return "DBProcessor"; }

	void					ProcessToDatabase( void );
	void					EntryPoint( void );
    bool		            BuildProcessQuery( bool bPrintStatsOnly = false );
	dbQuery_state			GetState() { return m_iState; }
	const char*				GetGameName();

private:
	void					Setup( void );
	bool					DataLeft();

    mysqlpp::Connection     m_sqlConn;
    GameStats*              m_pGameStats;
	bool					m_bDataLeft;
	dbQuery_state			m_iState;
	char					m_sQryString[1024];
};

#endif // DBPROCESSOR_H
