#ifndef __COMMON_H__
#define __COMMON_H__

typedef unsigned char	byte;
typedef uint16_t		uint16;
typedef uint32_t		uint32;

#define	V_TYPE_BIT		0x01
#define V_TYPE_BYTE		0x02
#define V_TYPE_CHAR		0x03
#define V_TYPE_WORD		0x04
#define	V_TYPE_INT		0x05
#define V_TYPE_DWORD	0x06
#define V_TYPE_DINT		0x07
#define V_TYPE_REAL		0x08

#define PARAFUNC_CONN   0xf0
#define PARAFUNC_READ   0x04
#define PARAFUNC_WRITE  0x05

#define HEADERTYPE_JOB  0x01
#define HEADERTYPE_ACK  0x03

#define DTSIZE_BIT      0x03
#define DTSIZE_BYTE     0x04
#define DTSIZE_WORD     0x04
#define DTSIZE_DWORD    0x04
#define DTSIZE_STRING   0x09
#define DTSIZE_INT      0x05
#define DTSIZE_REAL     0x07
 
#define PITEMAREA_P     0x80
#define PITEMAREA_I     0x81
#define PITEMAREA_Q     0x82
#define PITEMAREA_M     0x83
#define PITEMAREA_DB    0x84
#define PITEMAREA_C     0x28
#define PITEMAREA_T     0x29

#define COTPTYPE_CR     0xe0
#define COTPTYPE_CC     0xd0
#define COTPTYPE_DT     0xf0


/* 变量信息 与协议item有关信息 */
typedef struct
{
	byte		type;			/* 变量类型 */
	uint16		dbno;			/* DB号 */
	byte		area;			/* 内存区域 */
	uint32		addrByte;		/* 地址字节部分 */
	byte		addrBit;		/* 地址位部分 */
	uint16		length;			/* 数据长度(有多少个此类型变量) */
	void		*data;			/* 数据缓存 */
}S_Variable_Item;

/* 变量信息 */
typedef struct
{
	char			*group;		/* 变量组名 */
	char			*name;		/* 变量名 */
	byte			state;		/* 变量连接状态 */
	S_Variable_Item	*item		/* 变量信息 */
}Tag_Info;

/* PLC信息 */
typedef struct
{
	char	*ip;				/* PLC IP地址 */
	byte	rack;				/* CPU 机架号 */
	byte	slot;				/* CPU 插槽号 */
	byte	state;				/* PLC 连接状态 */
}PLC_Info;

/* 协议参数信息 */
typedef struct
{
	byte		COTPType		/* COTP包类型 */
	byte		PDUFunction		/* PDU包功能 */
	PLC_Info	*plcInfo		/* PLC信息 */
	Tag_Info	**items			/* 变量信息 */
}S_Msg_Para;


#endif

