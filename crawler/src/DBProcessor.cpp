#include <iostream>
#include <vector>
#include "DBProcessor.h"
#include "const.h"
#include "GameserverInfo.h"
#include <dbdriver.h>

using namespace std;

extern pthread_mutex_t muLog;

#define DB_HOST "localhost"
#define DB_USER "crawler"
#define DB_NAME "crawler"
#define DB_PASS "crawler"


DBProcessor::DBProcessor( ThreadFactory* pFactory, GameStats* pStats ) : ThreadedRequest( pFactory ), m_pGameStats( pStats )
{
	//SetParentClassName( "DBProcessor" );
	m_bDataLeft = true;
	m_iState = DBSTATE_NEW;
	m_sqlConn = mysqlpp::Connection(false);
}

DBProcessor::~DBProcessor()
{
    m_sqlConn.disconnect();
}

void DBProcessor::EntryPoint( void )
{
	ThreadedRequest::PreEntryPoint();
	Setup();
	ThreadedRequest::PostEntryPoint();
}

void DBProcessor::Setup()
{
    if(m_sqlConn.connect(DB_NAME, DB_HOST, DB_USER, DB_PASS))
    {
        Log("DBProcessor::Setup() connect successful!");
    }
    else
    {
        char sLog[256];
        snprintf(sLog, 256, "DBProcessor::Setup() connect failed! '%s'",m_sqlConn.error());
        Log(sLog);
    }

	m_pGameStats->ResetIterator();
}

void DBProcessor::ProcessToDatabase( void )
{
	while(DataLeft())
	{
		if(!m_sqlConn.connected())
		{
			Log("DBProcessor::ProcessToDatabase() database connection not active but data waiting to process!");
			break;
		}

		if (!BuildProcessQuery())
			return;

		std::cout << "trying to query: " << m_sQryString << std::endl;
		mysqlpp::Query query = m_sqlConn.query(m_sQryString);

		if(query.exec())
		{
			char sLog[2048];
			snprintf(sLog, 2048, "DBProcessor::ProcessToDatabase() query '%s' successful!", m_sQryString);
			Log(sLog);
		}
		else
		{
			char sLog[2048];
			snprintf(sLog, 2048, "DBProcessor::ProcessToDatabase() query failed while trying to query '%s'! error: '%s'", m_sQryString, query.error());
			Log(sLog);
		}
	}

	Log("DBProcessor::ProcessToDatabase() no data left!");
	m_iState = DBSTATE_DONE;
	m_pGameStats->SetState(GSSTATE_DONE);
}

bool DBProcessor::DataLeft()
{
    return m_bDataLeft;
}

bool DBProcessor::BuildProcessQuery()
{	    
	if(!DataLeft())
		return false;

	GameserverInfo* pInfo = m_pGameStats->GetNextServer();
	if (!pInfo)
	{
		m_bDataLeft = false;
		return false;
	}

	// TODO, escape all strings
	char sSaveServername[512];
	//mysqlpp::DBDriver* pDriver = m_sqlConn.driver();
	mysqlpp::DBDriver::escape_string_no_conn(sSaveServername, pInfo->m_sServername.c_str(), 512);

	snprintf(m_sQryString, 1024, "INSERT INTO `crawler`.`stats` \
( `id`, `type`, `version`, `servername`, `currentmap`, `gamedir`, `gamedesc`, `appid`, `playercount`, `maxplayers`, `botcount`, `dedicated`, `os`, `ispassworded`, `issecure`, `gameversion` ) \
VALUES ( NULL , '%u', '%u', '%s', '%s', '%s', '%s', '%u', '%u', '%u', '%u', '%u', '%u', '%u', '%u', '%s' );", pInfo->m_cType, pInfo->m_cVersion, sSaveServername, pInfo->m_sCurrentmap.c_str(), pInfo->m_sGamedir.c_str(), pInfo->m_sGamedesc.c_str(), pInfo->m_iAppid, pInfo->m_cPlayercount, pInfo->m_cMaxplayers, pInfo->m_cBotcount, pInfo->m_cDedicated, pInfo->m_cOS, pInfo->m_cISPassworded, pInfo->m_cISSecure, pInfo->m_sGameversion.c_str() );
	return true;
}

void DBProcessor::Loop( void )
{
	while (1)
	{
		Log("DBProcessor::Loop()");
		ProcessToDatabase();
		sleep(5);
	}
}

void DBProcessor::Log( const char* logMsg )
{
	int curThreads = GetActiveThreadNo();
	pthread_mutex_lock (&muLog);
	cout << "[" << time(NULL) << "|TF: " << curThreads << "] " << logMsg << endl;
	pthread_mutex_unlock (&muLog);
}

void DBProcessor::LogNoDeadLock( const char* logMsg )
{
	pthread_mutex_lock (&muLog);
	cout << "[" << time(NULL) << "|TF: ???] " << logMsg << endl;
	pthread_mutex_unlock (&muLog);
}

const char* DBProcessor::GetGameName()
{
	if(m_pGameStats)
		return m_pGameStats->GetGameName();
	return "UNDEFINED";
}
