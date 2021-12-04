
#define VIDEO_MEMORY 0xb8000

int kernel_start()
{
	char* videoMem = (char*)VIDEO_MEMORY;
	
	*videoMem = 'X';
	
	return 0;
}