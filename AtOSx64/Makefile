

AtOS.img : Bootload/Bootloader.bin Kernel/kernel.bin
	dd if=/dev/zero of=AtOS.img bs=1024 count=1440
	dd if=Bootload/Bootloader.bin of=AtOS.img conv=notrunc
	dd if=Kernel/kernel.bin of=AtOS.img bs=512 seek=1
	qemu-system-x86_64 AtOS.img


Bootload/Bootloader.bin:
	nasm -O0 -f bin -o Bootload/Bootloader.bin Bootload/Bootloader.asm

Kernel/kernel.bin : Kernel/kernel_start.o Kernel/kernel.o
	i686-elf-gcc -T linker.ld -o $@ -Wl,-Ttext,0x1000 -ffreestanding -O2 -nostdlib Kernel/kernel_start.o Kernel/kernel.o -lgcc

Kernel/kernel.o : Kernel/kernel.c
	i686-elf-gcc -c $< -o $@ -std=gnu99 -ffreestanding -O2 -Wall -Wextra

Kernel/kernel_start.o : Kernel/kernel_start.asm
	nasm $< -f elf -o $@


clean:
	rm Kernel/*.bin Kernel/*.o
	rm Bootload/*.bin