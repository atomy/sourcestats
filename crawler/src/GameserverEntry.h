#ifndef GAMESERVERENTRY_H
#define GAMESERVERENTRY_H

#include <string>
#include "const.h"


class GameserverEntry
{
public:
	GameserverEntry( servAddr );
	~GameserverEntry();

	servAddr	GetAddr( void ) { return m_stServaddr; }

protected:
	GameserverEntry( void ) { }

	servAddr m_stServaddr;
};

#endif // GAMESERVERENTRY_H
