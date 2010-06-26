#include <iostream>
#include "GameInfoRequest.h"
#include "GameserverInfo.h"

using namespace std;

void asQueryTest()
{
	servAddr sAddr;

	// 88.198.26.172:27045
    sAddr.ip1 = (unsigned char) 88;
    sAddr.ip2 = (unsigned char) 198;
    sAddr.ip3 = (unsigned char) 26;
    sAddr.ip4 = (unsigned char) 172;
    sAddr.port = (unsigned short) 27045;
    GameInfoRequest* req = new GameInfoRequest(sAddr);	
}

//int main()
//{
//	return 0;
//}
