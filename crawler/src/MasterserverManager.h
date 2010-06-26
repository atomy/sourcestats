#ifndef MASTERSERVERMANAGER_H
#define MASTERSERVERMANAGER_H

#include <vector>
#include "Masterserver.h"

class MasterserverManager
{
private:
	MasterserverManager() { };
	MasterserverManager( const MasterserverManager& cc );
	~MasterserverManager() { };

	static MasterserverManager* gMasterserverManager;

	std::vector <Masterserver*> m_vMasterserverList;
	//std::vector <GameserverInfo*> m_vServerInfos;

	//static void CleanupServerInfos( void );
	//void RequestServInfo( servAddr );

public:
	static MasterserverManager* getInstance();
	static void Destroy();

	void AddServer( Masterserver* mServer );
	void AddServer( const char* sServerstring );
	servAddr GetServerAdress( void );
	//void RequestAllServInfo( void );
	//void AddEntry( GameserverEntry* pEntry );
	//void Init( void );
};

#endif // MASTERSERVERMANAGER_H
