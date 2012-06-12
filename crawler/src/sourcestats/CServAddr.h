#ifndef CSERV_ADDR_H
#define CSERV_ADDR_H

#include <stdio.h>

class CServAddr
{
public:
	CServAddr() {
		ip1 = ip2 = ip3 = ip4 = 0;
		port = 0;
	}

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

	CServAddr& operator=(const char* buf) {
		sscanf(buf, "%hhu.%hhu.%hhu.%hhu:%hu", &ip1, &ip2, &ip3, &ip4, &port);
		return *this;
	}

	void toString(char* buf, size_t bufsize) {
		snprintf(buf, bufsize, "%hhu.%hhu.%hhu.%hhu:%hu", ip1, ip2, ip3, ip4, port);		
	}

	CServAddr* clone() {
		return new CServAddr(ip1, ip2, ip3, ip4, port);
	}

	void setIp(const char* strHostname) {
		sscanf(strHostname, "%hhu.%hhu.%hhu.%hhu", &ip1, &ip2, &ip3, &ip4);
	}

	void setPort(const char* strPort) {
		sscanf(strPort, "%hu", &port);
	}

public:
	unsigned char ip1;
	unsigned char ip2;
	unsigned char ip3;
	unsigned char ip4;
	unsigned short port;
};

#endif // CSERV_ADDR_H