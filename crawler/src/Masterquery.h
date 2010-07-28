#ifndef MASTERQUERY_H
#define MASTERQUERY_H

#include <string>
#include "const.h"
#include <boost/array.hpp>
#include <boost/asio.hpp>
#include "GameserverEntry.h"
#include "ThreadedRequest.h"
#include "SourceStats.h"

// this class is responsible for getting all servers for the given game
class Masterquery : public ThreadedRequest
{
public:
	Masterquery( ThreadFactory* );
	~Masterquery();

	void				SetMaster( servAddr );
	void				SetGame( const char* );
	void				EntryPoint( void );
	void				Query( void );
	void				AddEntry( GameserverEntry* );
	mqQuery_state		GetState( void ) { return m_iState; }
	void				ResetIterator( void );
	GameserverEntry*	GetNextServer( void );
    void                Exec( void );
	const char*			GetClassName( void ) { return "Masterquery"; }

    virtual void        Log( const char* logMsg );
	virtual bool		IsMasterquery( void ) { return true; }

protected:
	servAddr			RequestMore( boost::asio::ip::udp::socket* socket, servAddr gIp );
	servAddr			ParseMasterReply( const char* recvData, size_t len );

private:
	void				GetMasterIpString( char* );
	void				GetMasterPortString( char* );
	void				Finished( void );

	char				gameName[128];
	servAddr			masterAddr;
	mqQuery_state		m_iState;

	std::vector <GameserverEntry*>			m_vResultlist;
	std::vector <GameserverEntry*>::iterator m_geIT;
};

#endif // MASTERQUERY_H
