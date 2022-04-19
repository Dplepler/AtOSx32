all:	 clean DiskImages/AtOS.img

debug: clean DiskImages/AtOSDebug.img
gdb: 	 clean DiskImages/AtOSGDB.img

clean:
	rm -rf Kernel/*.bin Kernel/*.o
	rm -rf Bootload/*.bin

DiskImages/AtOS.img: makeImg
	qemu-system-x86_64 DiskImages/AtOS.img

DiskImages/AtOSDebug.img: makeImg
	qemu-system-x86_64 -hda DiskImages/AtOS.img -monitor stdio

DiskImages/AtOSGDB.img: makeImg
	qemu-system-x86_64 -S -s DiskImages/AtOS.img

makeImg: Bootload/stage0.bin Bootload/Bootloader.bin Kernel/kernel.bin
	dd if=/dev/zero of=DiskImages/AtOS.img bs=1024 count=1440
	dd if=Bootload/stage0.bin of=DiskImages/AtOS.img conv=notrunc
	dd if=Bootload/Bootloader.bin of=DiskImages/AtOS.img bs=512 seek=1 conv=notrunc
	dd if=Kernel/kernel.bin of=DiskImages/AtOS.img bs=512 seek=2 conv=notrunc
	

C_SOURCES = $(wildcard Kernel/*.c)

OBJ = $(C_SOURCES:.c=.o)

Kernel/kernel.bin : Kernel/kernel_start.o ${OBJ}
	i686-elf-gcc -T linker.ld -o $@ -ffreestanding -nostdlib Kernel/kernel_start.o $^ -lgcc

%.o : %.c
	i686-elf-gcc -c $< -o $@ -std=gnu99 -ffreestanding -Wall -Wextra

%.o : %.asm
	nasm $< -f elf32 -o $@

%.bin : %.asm
	nasm -O0 -f bin -o $@ $<



