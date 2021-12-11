#define VIDEO_MEMORY 0xB8000


int main()
{

	char* videoMem = (char*)VIDEO_MEMORY;
	
	*videoMem = 'X';

	
	return 0;
}
