#ifndef __S7PROTOCOL_H__
#define __S7PROTOCOL_H__

#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <netinet/in.h>

typedef unsigned char	byte;
typedef unsigned short	uint16;

#define PARAFUNC_CONN	0xf0
#define PARAFUNC_READ	0x04
#define PARAFUNC_WRITE	0x05

#define HEADERTYPE_JOB	0x01
#define HEADERTYPE_ACK	0x03

#define	PTSIZE_BIT		0x01
#define PTSIZE_BYTE		0x02
#define PTSIZE_CHAR		0x03
#define PTSIZE_WORD		0x04
#define	PTSIZE_INT		0x05
#define PTSIZE_DWORD	0x06
#define PTSIZE_DINT		0x07
#define PTSIZE_REAL		0x08

#define DTSIZE_BIT		0x03
#define DTSIZE_BYTE		0x04
#define DTSIZE_WORD		0x04
#define DTSIZE_DWORD	0x04
#define DTSIZE_STRING	0x09
#define DTSIZE_INT		0x05
#define DTSIZE_REAL		0x07

#define PITEMAREA_P		0x80
#define PITEMAREA_I		0x81
#define PITEMAREA_Q		0x82
#define PITEMAREA_M		0x83
#define PITEMAREA_DB	0x84
#define PITEMAREA_C		0x28
#define PITEMAREA_T		0x29

#define COTPTYPE_CR		0xe0
#define COTPTYPE_CC		0xd0
#define COTPTYPE_DT		0xf0

#define FCONVERT(type, buff) (type)(buff)
typedef struct
{
	byte		tranSize;
	uint16		dbno;
	byte		area;
	uint32_t	addrByte;
	byte		addrBit;
	uint16		length;
	void		*data;
}S_Para_Item;

typedef struct
{
	byte			COTPType;
	byte			rack;
	byte			slot;
	byte			PDUFunction;
	byte			itemsCount;
	S_Para_Item		**items;
}S_Msg_Para;

static uint16 CreateTPKT(uint16 length, byte *buff);
static uint16 CreateCOTP(S_Msg_Para *msgPara, byte *buff);
static uint16 CreatePDUHeader(uint16 paraLen, uint16 dataLen, byte *buff);
static uint16 CreatePDUParametersItems(S_Msg_Para *msgPara, byte *buff);
static uint16 CreatePDUParameters(S_Msg_Para *msgPara, byte *buff);
static uint16 CreatePDUData(S_Msg_Para *msgPara, byte *buff);
static uint16 CreatePDU(S_Msg_Para *msgPara, byte *buff);
uint16 CreateMessage(S_Msg_Para *msgPara, byte *buff);
uint16 GetData(S_Msg_Para *msgPara, byte *buff);
static uint16 ConvertData(S_Para_Item *msgPara);


#endif
