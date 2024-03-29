%macro IRQ 2

  global irq%1

  irq%1:
    cli
    push byte 0
    push  %2
    jmp irq_common_handler      

%endmacro


; Continue the IDT with interrupt requests
IRQ 0, 32
IRQ 1, 33
IRQ 2, 34
IRQ 3, 35
IRQ 4, 36
IRQ 5, 37
IRQ 6, 38
IRQ 7, 39
IRQ 8, 40
IRQ 9, 41
IRQ 10, 42
IRQ 11, 43
IRQ 12, 44
IRQ 13, 45
IRQ 14, 46
IRQ 15, 47

[extern irq_handler]

irq_common_handler:
  
  pusha
  push ds
  push es
  push fs
  push gs
  mov ax, 0x10
  mov ds, ax
  mov es, ax
  mov fs, ax
  mov gs, ax
  push esp
  call irq_handler  ; Call C function to handle an interrupt request
  pop eax   ; Saved stack pointer
  pop gs
  pop fs
  pop es
  pop ds
  popa
  add esp, 8
  iret


PARAM_EBX equ 0x4
PARAM_ECX equ 0x8
PARAM_EDX equ 0xC
PARAM_ESI equ 0x10
PARAM_EDI equ 0x14

[extern jmp_userland]
[extern service_routines]
[global syscall_dispatcher]
syscall_dispatcher:

  push edi
  push esi
  push edx
  push ecx
  push ebx

  mov ebx, service_routines
  xor ecx, ecx
  mov cl, ah
  mov eax, 4
  mul ecx
  add ebx, eax
  mov esi, dword [ebx]
  
  call esi
  add esp, 20

  iret


[extern create_file]
[global create_file_handler]
create_file_handler:
  
  mov edx, dword [esp + PARAM_EDX]
  mov esi, dword [esp + PARAM_ESI]
  mov edi, dword [esp + PARAM_EDI]

  push edx
  push esi
  push edi

  call create_file 
 
  add esp, 12
  ret


[extern terminal_write_string]
[global print]
print:

  push dword [esp + PARAM_ESI]
  call terminal_write_string
  add esp, 4
  ret

[extern key_pop]
[global getchar]
getchar:
  
  call key_pop
  ret

[extern malloc]
[global memalloc]
memalloc:
  
  push dword [esp + PARAM_EDX]
  call malloc
  add esp, 4
  ret

[extern realloc]
[global memrealloc]
memrealloc:
  
  mov edx, dword [esp + PARAM_EDX]
  mov esi, dword [esp + PARAM_ESI]
 
  push edx
  push esi
  call realloc
  add esp, 8
  ret

[extern terminal_write_int]
[global printh]
printh:
  
  mov ebx, dword [esp + PARAM_EBX]
  push 0x10
  push ebx
  call terminal_write_int
  add esp, 8
  ret


[extern write_file]
[global write_to_file]
write_to_file:

  mov edx, dword [esp + PARAM_EDX]
  mov edi, dword [esp + PARAM_EDI]
  mov esi, dword [esp + PARAM_ESI]

  push edx
  push edi
  push esi
  call write_file
  add esp, 12
  ret

[extern cat]
[global cat_to_file]
cat_to_file:

  mov edi, dword [esp + PARAM_EDI]
  mov esi, dword [esp + PARAM_ESI]

  push edi
  push esi
  call cat
  add esp, 8
  ret

