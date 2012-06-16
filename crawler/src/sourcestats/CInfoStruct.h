#ifndef C_INFO_STRUCT_H
#define C_INFO_STRUCT_H

#include <stdio.h>
#include <string>
#include "CServAddr.h"

using namespace std;

class CInfoStruct
{
public:
	CInfoStruct() {}
	~CInfoStruct() {}

	void								setServerName(const char* strServerName) {
		m_sServerName = strServerName;
	}

	const char*					getServerName() {
		return m_sServerName.c_str();
	}

	void								setGameVersion(const char* strBuf) {
		m_strGameVersion = strBuf;
	}

	void								setGameDesc(const char* strBuf) {
		m_strGameDesc = strBuf;
	}

	void								setGameDir(const char* strBuf) {
		m_strGameDir = strBuf;
	}

	void								setMap(const char* strBuf) {
		m_strMap = strBuf;
	}

	const char*					getMapName() {
		return m_strMap.c_str();
	}

	const char*					getGameDir() {
		return m_strGameDir.c_str();
	}

	const char*					getGameVersion() {
		return m_strGameVersion.c_str();
	}

	const char*					getGameDesc() {
		return m_strGameDesc.c_str();
	}

	void								toString(char* buf, size_t size) {
		char strAddr[64];		
		m_addr.toString(strAddr, 64);
		snprintf(buf, size, "%s name: '%s' map: '%s' %hhu/%hhu", strAddr, m_sServerName.c_str(), m_strMap.c_str(), playercount, maxplayers);
	}


public:
	unsigned char				type;
	unsigned char				version;	
	unsigned short			appid;
	unsigned char				playercount;
	unsigned char				maxplayers;
	unsigned char				bots;
	unsigned char				dedicated;
	unsigned char				os;
	unsigned char				password;
	unsigned char				secure;
	CServAddr						m_addr;

private:
	string							m_sServerName;
	string							m_strMap;
	string							m_strGameDir;
	string							m_strGameVersion;
	string							m_strGameDesc;
};

#endif // C_INFO_STRUCT_H