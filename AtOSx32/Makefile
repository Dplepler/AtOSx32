all:	 DiskImages/AtOS.img clean

debug: DiskImages/AtOSDebug.img clean
gdb: 	 DiskImages/AtOSGDB.img clean

clean:
	rm -rf  $(shell find Kernel/ -type f -name '*.o')
	rm -rf  $(shell find Kernel/ -type f -name '*.bin')
	rm -rf  $(shell find Bootload/ -type f -name '*.bin')

DiskImages/AtOS.img: makeImg
	qemu-system-x86_64 -drive file=DiskImages/AtOS.img,format=raw

DiskImages/AtOSDebug.img: makeImg
	qemu-system-x86_64 -hda DiskImages/AtOS.img -monitor stdio

DiskImages/AtOSGDB.img: makeImg
	qemu-system-x86_64 -S -s DiskImages/AtOS.img

makeImg: Bootload/stage0.bin Bootload/Bootloader.bin Kernel/kernel.bin
	dd if=/dev/zero of=DiskImages/AtOS.img bs=1024 count=3354
	dd if=Bootload/stage0.bin of=DiskImages/AtOS.img conv=notrunc
	dd if=Bootload/Bootloader.bin of=DiskImages/AtOS.img bs=512 seek=1 conv=notrunc
	dd if=Kernel/kernel.bin of=DiskImages/AtOS.img bs=512 seek=290 conv=notrunc
	

C_SOURCES = $(shell find Kernel/ -type f -name '*.c')
S_SOURCES = $(shell find Kernel/ -type f -name '*.asm')

OBJC = $(C_SOURCES:.c=.o)
OBJS = $(S_SOURCES:.asm=.o)

Kernel/kernel.bin : Kernel/kernel_start.o ${OBJS} ${OBJC}
	i686-elf-gcc -T linker.ld -o $@ -ffreestanding -nostdlib Kernel/kernel_start.o $^ -lgcc

%.o : %.c
	i686-elf-gcc -c $< -o $@ -I./Kernel -std=gnu99 -ffreestanding -Wall -Wextra 

%.o : %.asm
	nasm $< -f elf32 -o $@

%.bin : %.asm
	nasm -O0 -f bin -o $@ $<

