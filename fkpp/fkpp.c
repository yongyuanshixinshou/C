#include "fkpp.h"

static Tag_Info		*tags;
static int			tagsCount;
static pthread_t	ntid;
static float			t;
int main()
{
	
	GetTagsInfoFromFile();

	ConnectPLC("192.168.166.136", 102, 0, 2);
		
	int err = pthread_create(&ntid, NULL, RefreshTag, NULL);

	pthread_join(ntid, NULL);
}

static void PrintData()
{
	int i, j;

//	CLEAR();
//	MOVETO(0, 0);

	for(i = 0; i < tagsCount; i++)
	{	
		printf("name: %s\n", (tags + i) -> name);
		for(j = 0; j< (tags + i) -> item -> length; j++)
		{
			switch((tags + i) -> item ->type)
			{
				case PTSIZE_BIT:
				case PTSIZE_BYTE:
					printf("value[%d]=%d; ", j,((unsigned char *)((tags + i) -> item -> data))[j]);
					break;
				case PTSIZE_CHAR:
					printf("value[%d]=%c; ", j,((char *)((tags + i) -> item -> data))[j]);
					break;
				case PTSIZE_WORD:
					printf("value[%d]=%d; ", j,((uint16_t *)((tags + i) -> item -> data))[j]);
					break;
				case PTSIZE_INT:
					printf("value[%d]=%d; ", j,((int16_t *)((tags + i) -> item -> data))[j]);
					break;
				case PTSIZE_DWORD:
					printf("value[%d]=%d; ", j,((uint32_t *)((tags + i) -> item -> data))[j]);
					break;
				case PTSIZE_DINT:
					printf("value[%d]=%d; ", j,((int32_t *)((tags + i) -> item -> data))[j]);
					break;
				case PTSIZE_REAL:
					printf("value[%d]=%f; ", j,((float *)((tags + i) -> item -> data))[j]);
					break;
			}
		}
		printf("\n");
	}

	printf("time: %f\n", t);
	printf("-------------------------------------------------\n");

	fflush(stdout);
}

static void *RefreshTag(void *arg)
{
	S_Variable_Item	**items;
	int				i;
	struct timeval  s, e;

	items = (S_Variable_Item **)malloc(sizeof(S_Variable_Item *) * tagsCount);
	for(i = 0; i < tagsCount; i++)
	{
		items[i] = ((tags + i) -> item);
	}

	while(1)
	{
//		gettimeofday(&s, NULL);	
//		for(i = 0; i < 10; i++)
		{
			ReadFromPLC(items, tagsCount);
		}
//		gettimeofday(&e, NULL);
		//t = (long)(e.tv_sec - s.tv_sec);
//		t = 1000000 * (e.tv_sec - s.tv_sec)+ e.tv_usec - s.tv_usec;
//		t /= 1000000;
		PrintData();

		sleep(1);
	}
}

static int GetTagsInfoFromFile()
{
	int				i;
	FILE			*fp;
	char			*buff;
	char			*tmp;
	S_Variable_Item	*item;
	byte			*data;
	
	buff = (char *)malloc(1024);
	tags = (Tag_Info *)malloc(sizeof(Tag_Info) * 1024);

	if((fp = fopen("./addr_list.txt", "r")) == NULL)
		return -1;

	for(i = 0; fgets(buff, 512, fp) != NULL; i++)
	{
		tmp					= strtok(buff, ",");
		(tags + i) -> group	= (char *)malloc(strlen(tmp) + 1);
		memcpy((tags + i) -> group, tmp, strlen(tmp) + 1);

		tmp					= strtok(NULL, ",");
		(tags + i) -> name	= (char *)malloc(strlen(tmp) + 1);
		memcpy((tags + i) -> name, tmp, strlen(tmp) + 1);

		item = (S_Variable_Item *)malloc(sizeof(S_Variable_Item));
		item -> type		= GetVarType(strtok(NULL, ","));
		item -> memArea		= GetVarArea(strtok(NULL, ","));
		item -> length		= atoi(strtok(NULL, ","));
		item -> dbno		= atoi(strtok(NULL, ","));
		item -> addrByte	= atoi(strtok(NULL, ","));
		item -> addrBit		= atoi(strtok(NULL, ","));
		data = (byte *)malloc(GetDataLength(item -> type, item -> length));
		item -> data		= data;
		(tags + i) -> item	= item;

		if((tagsCount = i + 1) > 1024)
			tags = (Tag_Info *)realloc(tags, tagsCount);
	}
	free(buff);

	tags = (Tag_Info *)realloc(tags, tagsCount * sizeof(Tag_Info));
}

static int SearchTag(char *name, Tag_Info **tag)
{
	int	i;

	for(i = 0; i < tagsCount; i++)
	{
		if(strcmp((tags + i) -> name, name) == 0)
		{
			*tag = tags + i;
			return 0;
		}
	}
	
	return -1;
}

static uint16 GetDataLength(byte type, uint16 len)
{
	switch(type)
	{
		case V_TYPE_BIT:
		case V_TYPE_BYTE:
		case V_TYPE_CHAR:
			return len * 1;
		case V_TYPE_WORD:
		case V_TYPE_INT:
			return len * 2;
		case V_TYPE_DWORD:
		case V_TYPE_DINT:
		case V_TYPE_REAL:
			return len *4;
		defult:
			return -1;
	}
}

static byte GetVarType(char *strType)
{
	if(strcmp(strType, "bit") ==  0)
		return V_TYPE_BIT;
	else if(strcmp(strType, "byte") == 0)
		return V_TYPE_BYTE;
	else if(strcmp(strType, "char") == 0)
		return V_TYPE_CHAR;
	else if(strcmp(strType, "word") == 0)
		return V_TYPE_WORD;
	else if(strcmp(strType, "int") == 0)
		return V_TYPE_INT;
	else if(strcmp(strType, "dword") == 0)
		return V_TYPE_DWORD;
	else if(strcmp(strType, "dint") == 0)
		return V_TYPE_DINT;
	else if(strcmp(strType, "real") == 0)
		return V_TYPE_REAL;
	else
		return -1;
}

static byte GetVarArea(char *strArea)
{
	if(strcmp(strArea, "P") == 0)
		return V_AREA_P;
	else if(strcmp(strArea, "I") == 0)	
		return V_AREA_I;
	else if(strcmp(strArea, "Q") == 0)
		return V_AREA_Q;
	else if(strcmp(strArea, "M") == 0)
		return V_AREA_M;
	else if(strcmp(strArea, "DB") == 0)
		return V_AREA_DB;
	else
		return -1;
}
