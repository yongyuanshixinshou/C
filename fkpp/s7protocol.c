#include "s7protocol.h"
/*********
 * 建立TPKT包
 * length:	数据包总长度，TPKT + COPT + PDU
 * buff:	数据缓存
 * return:	缓存中的数据长度，小于0出错。
 *********************************************************/
static uint16 CreateTPKT(uint16 length, byte *buff)
{
	uint16 lenTPKT = 4;
	
	if(length < 0)
		return -1;		/* 出错 */

	*(buff + 0) = 0x03;
	*(buff + 1) = 0x00;
	*(buff + 2) = length >> 8;
	*(buff + 3) = length;

	return lenTPKT;
}

/*********
 * 建立COTP包
 * msgPara:	消息包相关信息
 * buff:	数据缓存
 * return:	缓存中的数据长度，小于0出错。
 *********************************************************/
static uint16 CreateCOTP(S_Msg_Para *msgPara, byte *buff)
{
	uint16 lenCOTP = 0;

	if(msgPara -> COTPType == COTPTYPE_CR)
	{
		lenCOTP = 18;

		*(buff + 0)		= lenCOTP - 1;
		*(buff + 1)		= msgPara -> COTPType;
		*(buff + 2)		= 0x00;
		*(buff + 3)		= 0x00;
		*(buff + 4)		= 0x00;
		*(buff + 5)		= 0x01;
		*(buff + 6)		= 0x00;
		*(buff + 7)		= 0xc1;
		*(buff + 8)		= 0x02;
		*(buff + 9)		= 0x01;
		*(buff + 10)	= 0x00;
		*(buff + 11)	= 0xc2;
		*(buff + 12)	= 0x02;
		*(buff + 13)	= 0x01;
		*(buff + 14)	= msgPara -> rack >> 5 | msgPara -> slot;
		*(buff + 15)	= 0xc0;
		*(buff + 16)	= 0x01;
		*(buff + 17)	= 0x09;
	}
	else if(msgPara -> COTPType == COTPTYPE_DT)
	{
		lenCOTP = 3;

		*(buff + 0)		= lenCOTP -1;
		*(buff + 1)		= msgPara -> COTPType;
		*(buff + 2)		= 0x80;
	}
	else
	{
		return -1;		/* 出错 */
	}
	
	return lenCOTP;
}

/*********
 * 建立PDU包Header
 * paraLen:	PDU包Parameters长度
 * dataLen:	PDU包Data长度
 * buff:	数据缓存
 * return:	缓存中的数据长度，小于0出错。
 *********************************************************/
static uint16 CreatePDUHeader(uint16 paraLen, uint16 dataLen, byte *buff)
{
	uint16 lenPDUHeader = 10;

	if(paraLen < 0 || dataLen < 0)
		return -1;		/* 出错 */

	*(buff + 0)	= 0x32;
	*(buff + 1)	= HEADERTYPE_JOB;
	*(buff + 2)	= 0x00;
	*(buff + 3)	= 0x00;
	*(buff + 4)	= 0x00;
	*(buff + 5)	= 0x00;
	*(buff + 6)	= paraLen >> 8;
	*(buff + 7)	= paraLen;
	*(buff + 8)	= dataLen >> 8;
	*(buff + 9)	= dataLen;

	return lenPDUHeader;
}

/*********
 * 建立PDU包Parameters Items
 * msgPara:	消息包相关信息
 * buff:	数据缓存
 * return:	缓存中的数据长度，小于0出错。
 *********************************************************/
static uint16 CreatePDUParametersItems(S_Msg_Para *msgPara, byte *buff)
{
	uint16			lenItems = 12 * msgPara -> itemsCount;	/* Item 固定长度为12字节 */
	S_Para_Item		*itemTmp;
	
	byte *t = buff;
	int i = 0;
	for(i; i < msgPara -> itemsCount; i++)
	{
		itemTmp = *(msgPara -> items + i);
		*(buff + 0)		= 0x12;
		*(buff + 1)		= 0x0a;
		*(buff + 2)		= 0x10;
		*(buff + 3)		= itemTmp -> tranSize;
		*(buff + 4)		= itemTmp -> length >> 8;
		*(buff + 5)		= itemTmp -> length;
		*(buff + 6)		= itemTmp -> dbno >> 8;
		*(buff + 7)		= itemTmp -> dbno;
		*(buff + 8)		= itemTmp -> area;
		*(buff + 9)		= itemTmp -> addrByte >> 13;
		*(buff + 10)	= itemTmp -> addrByte >> 5;
		*(buff + 11)	= itemTmp -> addrByte << 3 | itemTmp -> addrBit;

		buff	+= 12 ;
	}

	return lenItems;
}

/*********
 * 建立PDU包Parameters 
 * msgPara:	消息包相关信息
 * buff:	数据缓存
 * return:	缓存中的数据长度，小于0出错。
 *********************************************************/
static uint16 CreatePDUParameters(S_Msg_Para *msgPara, byte *buff)
{
	uint16	lenPara = 0;

	uint16	lenItems;

	switch(msgPara -> PDUFunction)
	{
		case PARAFUNC_CONN:
			lenPara = 8;

			*(buff + 0)	= msgPara -> PDUFunction;
			*(buff + 1)	= 0x00;
			*(buff + 2)	= 0x00;
			*(buff + 3)	= 0x01;
			*(buff + 4)	= 0x00;
			*(buff + 5)	= 0x01;
			*(buff + 6)	= 0x07;
			*(buff + 7)	= 0x80;

			return lenPara;

		case PARAFUNC_WRITE:
		case PARAFUNC_READ:
			
			*(buff + 0)	= msgPara -> PDUFunction;
			*(buff + 1)	= msgPara -> itemsCount;
		
			lenItems = CreatePDUParametersItems(msgPara, buff + 2);
			
			lenPara = 2 + lenItems;

			return lenPara;
		
		defult:
			return 0;
	}
}

/*********
 * 建立PDU包Data
 * msgPara:	消息包相关信息
 * buff:	数据缓存
 * return:	缓存中的数据长度，小于0出错。
 *********************************************************/
static uint16 CreatePDUData(S_Msg_Para *msgPara, byte *buff)
{
	uint16			tranSize;
	uint16			tranLen;
	uint16			lenData = 0;
	S_Para_Item		*itemTmp;

	if(msgPara -> PDUFunction != PARAFUNC_WRITE)
		return 0;
	
	int i = 0;
	for(i; i < msgPara -> itemsCount; i++)
	{
		//itemTmp = msgPara -> items + i;
		itemTmp = *(msgPara -> items + i);
		switch(itemTmp -> tranSize)
		{
			case PTSIZE_BIT:
				tranSize	= DTSIZE_BIT;
				tranLen		= 0x01;
				break;
			case PTSIZE_INT:
				tranSize	= DTSIZE_INT;
				tranLen		= itemTmp -> length * 16;
				break;
			case PTSIZE_CHAR:
				tranSize	= DTSIZE_STRING;
				tranLen		= itemTmp -> length * 8;
				break;
			case PTSIZE_BYTE:
				tranSize	= DTSIZE_BYTE;
				tranLen		= itemTmp -> length * 8;
				break;
			case PTSIZE_WORD:
				tranSize	= DTSIZE_WORD;
				tranLen		= itemTmp -> length * 16;
				break;
			case PTSIZE_DWORD:
				tranSize	= DTSIZE_DWORD;
				tranLen		= itemTmp -> length * 32;
				break;
			case PTSIZE_REAL:
				tranSize	= DTSIZE_REAL;
				//tranLen		= itemTmp -> length * 32;
				tranLen		= itemTmp -> length * 4;
				break;
		}	

		buff += lenData;

		*(buff + 0)	= 0x00;
		*(buff + 1)	= tranSize;
		*(buff + 2)	= tranLen >> 8;
		*(buff + 3)	= tranLen;
		
		memcpy(buff + 4, itemTmp -> data, tranLen / 8);

		lenData += 4 + tranLen / 8;	//Data数据包长度
	}

	return lenData;
}

/*********
 * 建立PDU包
 * msgPara:	消息包相关信息
 * buff:	数据缓存
 * return:	缓存中的数据长度，小于0出错。
 *********************************************************/
static uint16 CreatePDU(S_Msg_Para *msgPara, byte *buff)
{
	uint16	dataLen;
	uint16	paraLen;
	uint16	headerLen;
	uint16	lenPDU;

	if(msgPara -> COTPType != COTPTYPE_DT)
		return 0;

	headerLen	= CreatePDUHeader(0, 0, buff);

	buff += headerLen;
	paraLen		= CreatePDUParameters(msgPara, buff);

	buff += paraLen;
	dataLen		= CreatePDUData(msgPara, buff);

	buff -= (paraLen + headerLen);
	headerLen	= CreatePDUHeader(paraLen, dataLen, buff);
	
	return dataLen + paraLen + headerLen;
}

/*********
 * 建立Msg
 * msgPara:	消息包相关信息
 * buff:	数据缓存
 * return:	缓存中的数据长度，小于0出错。
 *********************************************************/
uint16 CreateMessage(S_Msg_Para *msgPara, byte *buff)
{
	uint16	lenPDU;
	uint16	lenCOTP;
	uint16	lenTPKT;
	
	lenTPKT		= CreateTPKT(0, buff);

	buff += lenTPKT;
	lenCOTP		= CreateCOTP(msgPara, buff);

	buff += lenCOTP;
	lenPDU		= CreatePDU(msgPara, buff);

	buff -= (lenCOTP + lenTPKT);
	lenTPKT		= CreateTPKT(lenPDU + lenCOTP + lenTPKT, buff);

	return lenPDU + lenCOTP + lenTPKT;
}

/*********
 * 获取数据
 * msgPara:	消息包相关信息
 * buff:	msg数据缓存
 * return:	缓存中的数据部分长度，小于0出错。
 *********************************************************/
uint16 GetData(S_Msg_Para *msgPara, byte *buff)
{
	uint16 len = 0;
	uint16 dataLen = 0;
	int i = 0;
	for(i; i < msgPara -> itemsCount; i++)
	{
		if(buff[7 + 14 + len] != 0xff)
		{
			printf("recv write variable error : %02x\n", buff[7 + 14 + len]);
		}
		else
		{
			if(buff[7 + 15 + len] == 0x07)
			{
				dataLen =((uint16_t)buff[7 + 14 + 2 + len] << 8 | buff[7 + 14 + 3 + len]);
			}
			else
			{
				dataLen =((uint16_t)buff[7 + 14 + 2 + len] << 8 | buff[7 + 14 + 3 + len]) >> 3;
			}
			memcpy((*(msgPara -> items + i)) -> data, buff + 7 + 14 + 4 + len, dataLen);

			ConvertData(*(msgPara -> items +i));

			len += (dataLen + 4);
		}
	}
	return (len - 4 * msgPara -> itemsCount);
}

static uint16 ConvertData(S_Para_Item *paraItems)
{
	int i;

	for(i = 0; i < paraItems -> tranSize; i++)
	{
		switch(paraItems ->tranSize)
		{
			case PTSIZE_WORD:
			case PTSIZE_INT:
					((uint16_t *)(paraItems -> data))[i] = ntohs(((uint16_t *)(paraItems -> data))[i]);
				 
				break;
			case PTSIZE_DWORD:
			case PTSIZE_DINT:
			case PTSIZE_REAL:
					((uint32_t *)(paraItems -> data))[i] = ntohl(((uint32_t *)(paraItems -> data))[i]);
					break;
						
		}
	}
}
