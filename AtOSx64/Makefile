

DiskImages/AtOS.img : Bootload/Bootloader.bin Kernel/kernel.bin
	dd if=/dev/zero of=AtOS.img bs=1024 count=1440
	dd if=Bootload/Bootloader.bin of=AtOS.img conv=notrunc
	dd if=Kernel/kernel.bin of=AtOS.img bs=512 seek=1
	qemu-system-x86_64 AtOS.img


Bootload/Bootloader.bin:
	nasm -O0 -f bin -o Bootload/Bootloader.bin Bootload/Bootloader.asm

Kernel/kernel.bin : Kernel/kernel_start.o Kernel/kernel.o
	ld -m elf_i386 -s -o Kernel/kernel.bin -Ttext 0x1000 $^ --oformat binary

Kernel/kernel.o : Kernel/kernel.c
	gcc -m32 -fno-pie -ffreestanding -c $< -o $@

Kernel/kernel_start.o : Kernel/kernel_start.asm
	nasm $< -f elf -o $@


clean:
	rm *.bin *.o