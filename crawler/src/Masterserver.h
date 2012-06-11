#ifndef MASTERSERVER_H
#define MASTERSERVER_H

#include <string>
#include "const.h"


class Masterserver
{
public:
	Masterserver( servAddr );
	~Masterserver();

	servAddr getAddr( void ) { return m_stServaddr; }

	char*	toString( void );

protected:
	Masterserver( void ) { }

private:
	servAddr m_stServaddr;
};

#endif // MASTERSERVER_H
