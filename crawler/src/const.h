#ifndef CONST_H
#define CONST_H

#include <stdio.h>

enum query_state
{
	STATE_NONE = 0,
	STATE_NEW,
	STATE_INPROGRESS,
	STATE_WAITINGFORREPLY,
	STATE_DONE,
	STATE_FAILED
};

/**
 * parse reply udp packet:
 * Type      byte    Should be equal to 'I' (0x49)
 * Version  byte    Network version. 0x07 is the current Steam version. Goldsource games will return 48 (0x30), also refered to as protocol version.
 * Server Name  string  The Source server's name, eg: "Recoil NZ CS Server #1"
 * Map  string  The current map being played, eg: "de_dust"
 * Game Directory   string  The name of the folder containing the game files, eg: "cstrike"
 * Game Description     string  A friendly string name for the game type, eg: "Counter Strike: Source"
 * AppID    short   Steam Application ID
 * Number of players    byte    The number of players currently on the server
 * Maximum players  byte    Maximum allowed players for the server
 * Number of bots   byte    Number of bot players currently on the server
 * Dedicated    byte    'l' for listen, 'd' for dedicated, 'p' for SourceTV
 * OS   byte    Host operating system. 'l' for Linux, 'w' for Windows
 * Password     byte    If set to 0x01, a password is required to join this server
 * Secure   byte    if set to 0x01, this server is VAC secured
 * Game Version     string  The version of the game, eg: "1.0.0.22" 
 */

struct as_info
{
    unsigned char type;
    unsigned char version;
    char servername[255];
    char map[64];
    char gamedir[128];
    char gamedesc[255];
    unsigned short appid;
    unsigned char playercount;
    unsigned char maxplayers;
    unsigned char bots;
    unsigned char dedicated;
    unsigned char os;
    unsigned char password;
    unsigned char secure;
    char gameversion[32];
};

typedef struct {
    unsigned char ip1;
    unsigned char ip2;
    unsigned char ip3;
    unsigned char ip4;
    unsigned short port;
} servAddr;

// converts the given servAddr struct to a string
void servAddr2String( char* sAddr, size_t size, servAddr stServaddr );
void servAddr2Ip( char* sAddr, size_t size, servAddr stServaddr );
void servAddr2Port( char* sAddr, size_t size, servAddr stServaddr );

#endif // CONST_H
