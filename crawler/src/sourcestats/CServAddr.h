#ifndef CSERV_ADDR_H
#define CSERV_ADDR_H

#include <stdio.h>

class CServAddr
{
public:
	CServAddr() {}
	CServAddr(unsigned char _ip1, unsigned char _ip2, unsigned char _ip3, unsigned char _ip4, unsigned short _port) {
		ip1 = _ip1;
		ip2 = _ip2;
		ip3 = _ip3;
		ip4 = _ip4;
		port = _port;
	}
	CServAddr(const char* buf) {
		sscanf(buf, "%hhu.%hhu.%hhu.%hhu:%hu", &ip1, &ip2, &ip3, &ip4, &port);
	}
	~CServAddr() {}

	void toString(char* buf, size_t bufsize) {
		snprintf(buf, bufsize, "%hhu.%hhu.%hhu.%hhu:%hu", ip1, ip2, ip3, ip4, port);		
	}

	CServAddr* clone() {
		return new CServAddr(ip1, ip2, ip3, ip4, port);
	}

public:
	unsigned char ip1;
	unsigned char ip2;
	unsigned char ip3;
	unsigned char ip4;
	unsigned short port;
};

#endif // CSERV_ADDR_H