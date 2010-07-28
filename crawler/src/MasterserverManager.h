#ifndef MASTERSERVERMANAGER_H
#define MASTERSERVERMANAGER_H

#include <vector>
#include "Masterserver.h"
#include <pthread.h>

class MasterserverManager
{
private:
	MasterserverManager() { pthread_mutex_init(&m_masterMutex, NULL); };
	MasterserverManager( const MasterserverManager& cc );
	~MasterserverManager() { };

	static MasterserverManager* gMasterserverManager;

	std::vector <Masterserver*> m_vMasterserverList;
	//std::vector <GameserverInfo*> m_vServerInfos;

	//static void CleanupServerInfos( void );
	//void RequestServInfo( servAddr );
	pthread_mutex_t					m_masterMutex;

public:
	static MasterserverManager* getInstance();
	static void Destroy();

	void				AddServer( Masterserver* mServer );
	void				AddServer( const char* sServerstring );
	servAddr			GetServerAdress( void );
};

#endif // MASTERSERVERMANAGER_H
