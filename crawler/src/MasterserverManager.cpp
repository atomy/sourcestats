#include "MasterserverManager.h"
#include <iostream>
#include "const.h"
#include "Masterserver.h"
#include <stdlib.h>

MasterserverManager* MasterserverManager::gMasterserverManager = NULL;

MasterserverManager* MasterserverManager::getInstance( void )
{
	if ( !gMasterserverManager )
	{
		gMasterserverManager = new MasterserverManager();
	}
	return gMasterserverManager;
}

void MasterserverManager::Destroy()
{
	delete gMasterserverManager;
	gMasterserverManager = NULL;
}

void MasterserverManager::AddServer( const char* sServerstring )
{
	servAddr stServaddr;
	int ret = sscanf( sServerstring, "%u.%u.%u.%u:%u", (unsigned int*)&stServaddr.ip1, (unsigned int*)&stServaddr.ip2, (unsigned int*)&stServaddr.ip3, (unsigned int*)&stServaddr.ip4, (unsigned int*)&stServaddr.port );

	if ( ret == 5 )
	{
		Masterserver* mServer = new Masterserver( stServaddr );
		AddServer( mServer );
	}
	else
		fprintf( stderr, "MasterserverManager::AddServer() tried to add malformed server ip:port -- %s\n", sServerstring );
}

void MasterserverManager::AddServer( Masterserver* mServer )
{
	char servString[128];
	servAddr2String( servString, 128, mServer->getAddr() );
	std::cout << "[" << time(NULL) << "] MasterserverManager::AddServer() added new master server: " << servString << std::endl;

    pthread_mutex_lock(&m_masterMutex);
	m_vMasterserverList.push_back(mServer);
	pthread_mutex_unlock(&m_masterMutex);
}

servAddr MasterserverManager::GetServerAdress( void )
{
    pthread_mutex_lock(&m_masterMutex);
	Masterserver* mServer = m_vMasterserverList.front(); // TODO, get a random one, make some quality checks to use the best quality one, lowest query rate etc.
	pthread_mutex_unlock(&m_masterMutex);
	return mServer->getAddr();
}
