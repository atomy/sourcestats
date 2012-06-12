#ifndef C_INFO_STRUCT_H
#define C_INFO_STRUCT_H

#include <stdio.h>
#include <string>

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

private:
	string							m_sServerName;
	string							m_strMap;
	string							m_strGameDir;
	string							m_strGameVersion;
	string							m_strGameDesc;
};

#endif // C_INFO_STRUCT_H