#ifndef __S7COMM_H__
#define __S7COMM_H__

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
	
#include "s7protocol.h"

#define V_TYPE_BIT		0x01
#define V_TYPE_BYTE		0x02
#define V_TYPE_CHAR		0x03
#define V_TYPE_WORD		0x04
#define V_TYPE_INT		0x05
#define V_TYPE_DWORD	0x06
#define V_TYPE_DINT		0x07
#define V_TYPE_REAL		0x08

#define V_AREA_P		0x80
#define V_AREA_I		0x81
#define V_AREA_Q		0x82
#define V_AREA_M		0x83
#define V_AREA_DB		0x84
#define V_AREA_C		0x28
#define V_AREA_T		0x29

typedef struct
{
	byte		type;
	uint16		dbno;
	byte		memArea;
	uint32_t	addrByte;
	byte		addrBit;
	uint16		length;
	byte		*data;
}S_Variable_Item;


void PrintArray(char *msg, byte *buff, int len);
static int ConnectSocket(char *ip, int port);
int ConnectPLC(char *ip, int port, char rack, char slot);
int ReadFromPLC(S_Variable_Item **varItems, byte itemsCount);
int WriteToPLC(S_Variable_Item **varItems, byte itemsCount);
static int SendMsg(S_Msg_Para *msgPara);
static int RecvMsg(S_Msg_Para *msgPara);












































#endif
