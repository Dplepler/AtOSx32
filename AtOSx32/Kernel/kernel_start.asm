; This file should always be linked as the first binary of a kernel
; That way we can always choose which Kernel function will be called first
; In this case it's the main function of the kernel

[bits 32]
[extern kmain] 		; Tell NASM that the main kernel function exists and will be found when we link files

call kmain
jmp $

