#ifndef MASTERQUERY_H
#define MASTERQUERY_H

#include <string>
#include "const.h"
#include <boost/array.hpp>
#include <boost/asio.hpp>
#include "GameserverEntry.h"
#include "ThreadedRequest.h"
#include "SourceStats.h"


class Masterquery : public ThreadedRequest
{
public:
	Masterquery();
	~Masterquery();

	void				SetMaster( servAddr );
	void				SetGame( const char* );
	void				Exec( void );
	void				Query( void );
	void				AddEntry( GameserverEntry* );
	query_state			GetStatus( void ) { return m_iState; }
	void				ResetIterator( void );
	GameserverEntry*	GetNextServer( void );

protected:
	servAddr			RequestMore( boost::asio::ip::udp::socket* socket, servAddr gIp );
	servAddr			ParseMasterReply( const char* recvData, size_t len );

private:
	void				GetMasterIpString( char* );
	void				GetMasterPortString( char* );
	void				Finished( void );

	char				gameName[128];
	servAddr			masterAddr;
	query_state			m_iState;
	GameserverEntry*	m_pParent;

	vector <GameserverEntry*>			m_vResultlist;
	vector <GameserverEntry*>::iterator m_geIT;
};

#endif // MASTERQUERY_H
