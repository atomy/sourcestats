#ifndef GAMESERVERINFO_H
#define GAMESERVERINFO_H

#include <string>
#include "const.h"
#include "GameserverEntry.h"


class GameserverInfo : public GameserverEntry
{
public:
	GameserverInfo(servAddr);
	//GameserverInfo(const GameserverInfo& obj);
	~GameserverInfo();

public: 
	// reply data, see http://developer.valvesoftware.com/wiki/Server_Queries
	unsigned char 	m_cType;
	unsigned char 	m_cVersion;
	std::string		m_sServername;
	std::string		m_sCurrentmap;
	std::string		m_sGamedir;
	std::string		m_sGamedesc;
	unsigned short	m_iAppid;
	unsigned char	m_cPlayercount;
	unsigned char 	m_cMaxplayers;
	unsigned char	m_cBotcount;
	unsigned char	m_cDedicated;
	unsigned char	m_cOS;
	unsigned char	m_cISPassworded;
	unsigned char	m_cISSecure;
	std::string		m_sGameversion;

public:
	void SetType( unsigned char type ) { m_cType = type; }
	void SetVersion( unsigned char version ) { m_cVersion = version; }
	void SetServerName( std::string sname ) { m_sServername = sname; }
	void SetCurrentMap( std::string map ) { m_sCurrentmap = map; }
	void SetGameDir( std::string gamedir ) { m_sGamedir = gamedir; }
	void SetGameDesc( std::string gamedesc ) { m_sGamedesc = gamedesc; }
	void SetAppid( unsigned short appid ) { m_iAppid = appid; }
	void SetPlayerCount( unsigned char playercount ) { m_cPlayercount = playercount; }
	void SetMaxPlayers( unsigned char maxplayers ) { m_cMaxplayers = maxplayers; }
	void SetBotCount( unsigned char botcount ) { m_cBotcount = botcount; }
	void SetDedicated( unsigned char ded ) { m_cDedicated = ded; }
	void SetOS( unsigned char os ) { m_cOS = os; }
	void SetPassworded( unsigned char pw ) { m_cISPassworded = pw; }
	void SetSecure( unsigned char secure ) { m_cISSecure = secure; }
	void SetGameversion( std::string version ) { m_sGameversion = version; }
};

#endif // GAMESERVERINFO_H
