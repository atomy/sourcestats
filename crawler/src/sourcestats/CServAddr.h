#ifndef CSERV_ADDR_H
#define CSERV_ADDR_H

#include <stdio.h>

class CServAddr
{
public:
	CServAddr() {}
	CServAddr(const char* buf) {
		sscanf(buf, "%hhu.%hhu.%hhu.%hhu:%hu", &ip1, &ip2, &ip3, &ip4, &port);
	}
	~CServAddr() {}

	void toString(char* buf, size_t bufsize) {
		snprintf(buf, bufsize, "%hhu.%hhu.%hhu.%hhu:%hu", ip1, ip2, ip3, ip4, port);		
	}

public:
	unsigned char ip1;
	unsigned char ip2;
	unsigned char ip3;
	unsigned char ip4;
	unsigned short port;
};

#endif // CSERV_ADDR_H