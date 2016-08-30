#include "s7comm.h"
#include <stdlib.h>



int main()
{
	S_Variable_Item *item;
	byte			*data;

	data = (byte *)malloc(sizeof(byte) * 2);
	item = (S_Variable_Item *)malloc(sizeof(S_Variable_Item));
	item -> type = 0x05;
	item -> dbno = 0;
	item -> memArea = 0x83;
	item -> addrByte = 0;
	item -> addrBit = 0;
	item -> length = 1;
	item -> data = data;
	

	ConnectPLC("192.168.68.129", 102, 0, 2);
	ReadFromPLC(&item, 1);
	printf("data: %d\n", ((int16_t *)data)[0]);
	free(item);
}
