
all: DiskImages/AtOS.img

DiskImages/AtOS.img : Bootload/Bootloader.bin Kernel/kernel.bin
	dd if=/dev/zero of=DiskImages/AtOS.img bs=1024 count=1440
	dd if=Bootload/Bootloader.bin of=DiskImages/AtOS.img conv=notrunc
	dd if=Kernel/kernel.bin of=DiskImages/AtOS.img bs=512 seek=1 conv=notrunc
	qemu-system-x86_64 DiskImages/AtOS.img


C_SOURCES = $(wildcard Kernel/*.c)

OBJ = $(C_SOURCES:.c=.o)

Kernel/kernel.bin : Kernel/kernel_start.o ${OBJ}
	i686-elf-gcc -T linker.ld -o $@ -ffreestanding -nostdlib Kernel/kernel_start.o $^ -lgcc

%.o : %.c
	i686-elf-gcc -c $< -o $@ -std=gnu99 -ffreestanding -Wall -Wextra

%.o : %.asm
	nasm $< -f elf -o $@

%.bin : %.asm
	nasm -O0 -f bin -o $@ $<
	


clean:
	rm -rf Kernel/*.bin Kernel/*.o
	rm -rf Bootload/*.bin