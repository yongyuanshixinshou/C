#ifndef __FKPP_H__
#define __FKPP_H__

#include <pthread.h>
#include <fcntl.h>
#include "s7comm.h"

//#define CONVERT(type, buff) ((type) (buff))

typedef struct
{
	char			*group;
	char			*name;
	S_Variable_Item	*item;
}Tag_Info;


//int ReadTag(char *name, void* buff);
static void *RefreshTag(void *arg);
static void PrintData();
static int GetTagsInfoFromFile();
static int SearchTag(char *name, Tag_Info **tag);
static byte GetVarType(char *strType);
static byte GetVarArea(char *strArea);
static uint16 GetDataLength(byte type, uint16 len);





#endif
