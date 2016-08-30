#include "s7comm.h"

static int fd;

/*********
 * 打印字节数组，每8个字节一组
 * msg:		描述信息
 * buff:	数据缓存
 * len:		缓存中的数据长度
 *********************************************************/
void PrintArray(char *msg, byte *buff, int len)
{/*
	printf("%s\n", msg);
	printf("n = %d\n", len);
	int i;
	for(i = 0; i < len; i ++)
	{
		if(i != 0 && i % 8 == 0)
			printf("\n");
		printf("0x%02x ", buff[i]);
	}
	printf("\n");
*/
}

/*********
 * 连接到Socket
 * ip:		IP地址
 * port:	端口号
 *********************************************************/
static int ConnectSocket(char *ip, int port)
{
	struct sockaddr_in		addr;
	int						val;

	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	inet_pton(AF_INET, ip, &addr.sin_addr);

	if((fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		printf("socket create error\n");
	return -1;
	}

	if(connect(fd,(struct sockaddr *)&addr, sizeof(addr)) < 0)
	{
		printf("connect server error\n");
		close(fd);
		return -1;
	}

	val = 1;
	if(setsockopt(fd,SOL_SOCKET, SO_KEEPALIVE, &val, sizeof(int)) < 0)
	{
		printf("setsockopt error\n");
		close(fd);
	}

	return 0;
}

/*********
 * 连接到PLC
 * ip:		IP地址
 * port:	端口号
 *********************************************************/
int ConnectPLC(char *ip, int port, char rack, char slot)
{
	int						n;
	S_Msg_Para				*msgPara;

	msgPara		= malloc (sizeof(S_Msg_Para));

	//连接到Socket
	ConnectSocket(ip, port);
	sleep(1);	
	//建立连接
	msgPara -> COTPType = COTPTYPE_CR;

	n = SendMsg(msgPara);
	n = RecvMsg(msgPara);

	//设置连接
	msgPara -> COTPType		= COTPTYPE_DT;
	msgPara -> PDUFunction	= PARAFUNC_CONN;
	msgPara -> rack			= rack;
	msgPara -> slot			= slot;

	n = SendMsg(msgPara);
	n = RecvMsg(msgPara);

	free(msgPara);

	return fd;
}

/*********
 * 从PLC读取数据
 * 
 * varItems:	Items信息
 * itemsCount:	Item数量
 *********************************************************/
int ReadFromPLC(S_Variable_Item **varItems, byte itemsCount)
{
	int			n;
	S_Msg_Para	*msgPara;
	
	msgPara		= malloc (sizeof(S_Msg_Para));

	msgPara -> COTPType		= COTPTYPE_DT;
	msgPara -> PDUFunction	= PARAFUNC_READ;
	msgPara -> itemsCount	= itemsCount;
	msgPara -> items		= (S_Para_Item **)varItems;

	n = SendMsg(msgPara);
	n = RecvMsg(msgPara);

	free(msgPara);

	return n;
}

/*********
 * 写数据到PLC
 * 
 * varItems:	Items信息
 * itemsCount:	Item数量
 *********************************************************/
int WriteToPLC(S_Variable_Item **varItems, byte itemsCount)
{
	int			n;
	S_Msg_Para	*msgPara;

	msgPara		= malloc (sizeof(S_Msg_Para));

	msgPara -> COTPType		= COTPTYPE_DT;
	msgPara -> PDUFunction	= PARAFUNC_WRITE;
	msgPara -> itemsCount	= itemsCount;
	msgPara -> items		= (S_Para_Item **)varItems;

	n = SendMsg(msgPara);
	n = RecvMsg(NULL);
	
	free(msgPara);

	return 0;
}

/*********
 * 发送数据包
 * 
 * msgPara:	Message数据包相关参数
 *********************************************************/
static int SendMsg(S_Msg_Para *msgPara)
{
	byte	*msgBuff;
	uint16	msgLen;

	msgBuff = (byte *)malloc(1024);

	msgLen = CreateMessage(msgPara, msgBuff);

	send(fd, msgBuff, msgLen, 0);

	PrintArray("Send:", msgBuff, msgLen);

	free(msgBuff);
}

/*********
 * 接收数据包
 * 
 * msgPara:	Message数据包相关参数
 *********************************************************/
static int RecvMsg(S_Msg_Para *msgPara)
{
	byte	*recvBuff;
	int		n;
	int		dataLen;

	recvBuff = (byte *)malloc(1024);
	
	n = recv(fd, recvBuff, 512, 0);

	PrintArray("Recv:", recvBuff, n);

	if(msgPara == NULL)
		return -1;

	if(recvBuff[7 + 12] != 0x04)
		return n;
	
	if(msgPara -> COTPType == COTPTYPE_DT && msgPara -> PDUFunction == PARAFUNC_READ)
	{
		dataLen = GetData(msgPara, recvBuff);
	}

	free(recvBuff);

	return dataLen;
}

