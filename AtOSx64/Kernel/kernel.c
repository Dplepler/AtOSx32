#define VIDEO_MEMORY 0xB8000

<<<<<<< HEAD

void test()
{
	

	char* videoMem = (char*)VIDEO_MEMORY;
	
	*videoMem = 'X';
}

int main()
{


	char* videoMem = (char*)VIDEO_MEMORY;
	
	*videoMem = 'E';
=======
int main();
void test();

void kernel_start()
{
	__asm__ __volatile__ (

	"call main\n\t"
	"jmp .\n\t"

	);
}


int main()
{


	char* videoMem = (char*)VIDEO_MEMORY;
	
	*videoMem = 'Z';
>>>>>>> e436d6e0013eb76772ac55b9671e0c4961e2c63d



	
	return 0;
}

<<<<<<< HEAD

=======
void test()
{
	

	char* videoMem = (char*)VIDEO_MEMORY;
	
	*videoMem = 'X';
}
>>>>>>> e436d6e0013eb76772ac55b9671e0c4961e2c63d
