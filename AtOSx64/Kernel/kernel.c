#define VIDEO_MEMORY 0xB8000

void kernel_start()
{
	__asm__ __volatile__ (

	"call main\n"
	"jmp .\n"

	);
}


int main()
{


	char* videoMem = (char*)VIDEO_MEMORY;
	
	*videoMem = 'Z';



	
	return 0;
}

void test()
{
	

	char* videoMem = (char*)VIDEO_MEMORY;
	
	*videoMem = 'X';
}
