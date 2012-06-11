#include "const.h"
#include "stdio.h"

void servAddr2String( char* sAddr, size_t size, servAddr stServaddr )
{	
	snprintf( sAddr, size, "%u.%u.%u.%u:%u", stServaddr.ip1, stServaddr.ip2, stServaddr.ip3, stServaddr.ip4, stServaddr.port );	
}

void servAddr2Ip( char* sAddr, size_t size, servAddr stServaddr )
{	
	snprintf( sAddr, size, "%u.%u.%u.%u", stServaddr.ip1, stServaddr.ip2, stServaddr.ip3, stServaddr.ip4 );	
}

void servAddr2Port( char* sAddr, size_t size, servAddr stServaddr )
{	
	snprintf( sAddr, size, "%u", stServaddr.port );	
}