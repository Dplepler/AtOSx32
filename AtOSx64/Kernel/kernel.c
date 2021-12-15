#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define VIDEO_MEMORY 0xB8000

int main()
{

	char* message = "Hello there David!";
	char* videoMem = (char*)VIDEO_MEMORY;
	*videoMem = 'E';

	return 0;
}


