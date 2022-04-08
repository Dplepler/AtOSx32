import os

commands = ["gcc -ffreestanding -c Kernel\\kernel.c -o Kernel\\kernel.o", "ld -r -Ttext 0x1000 -o Kernel\\kernel.out Kernel\\kernel.o"
, "objcopy -O binary -j .text Kernel\\kernel.out Kernel\\kernel.bin", "nasm -O0 -f bin -o Bootload\\Bootloader.bin Bootload\\Bootloader.asm"
, "dd if=/dev/zero of=AtOS.img bs=1024 count=1440", "dd if=bootload\\bootloader.bin of=AtOS.img", "dd if=kernel\\kernel.bin of=AtOS.img bs=512 seek=1"]

for command in commands:
	os.system(command)