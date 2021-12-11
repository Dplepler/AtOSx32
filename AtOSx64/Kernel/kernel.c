#define VIDEO_MEMORY 0xB8000


void test()
{
	

	char* videoMem = (char*)VIDEO_MEMORY;
	
	*videoMem = 'X';
}

int main()
{


	char* videoMem = (char*)VIDEO_MEMORY;
	
	*videoMem = 'E';



	
	return 0;
}


