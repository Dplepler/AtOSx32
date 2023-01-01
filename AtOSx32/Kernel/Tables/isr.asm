; Interrupt Service Routines


%macro ISR_NO_ERROR 1

  global isr%1

  isr%1:
    push byte 0
    push  %1
    jmp isr_common_handler      

%endmacro


%macro ISR_ERROR 1
 
  global isr%1

  isr%1:
    push %1
    jmp isr_common_handler

%endmacro

ISR_NO_ERROR 0
ISR_NO_ERROR 1
ISR_NO_ERROR 2
ISR_NO_ERROR 3
ISR_NO_ERROR 4
ISR_NO_ERROR 5
ISR_NO_ERROR 6
ISR_NO_ERROR 7
ISR_ERROR    8
ISR_NO_ERROR 9
ISR_ERROR    10
ISR_ERROR    11
ISR_ERROR    12
ISR_ERROR    13
ISR_ERROR    14
ISR_NO_ERROR 15
ISR_NO_ERROR 16
ISR_NO_ERROR 17
ISR_NO_ERROR 18
ISR_NO_ERROR 19
ISR_NO_ERROR 20
ISR_NO_ERROR 21
ISR_NO_ERROR 22
ISR_NO_ERROR 23
ISR_NO_ERROR 24
ISR_NO_ERROR 25
ISR_NO_ERROR 26
ISR_NO_ERROR 27
ISR_NO_ERROR 28
ISR_NO_ERROR 29
ISR_NO_ERROR 30
ISR_NO_ERROR 31

[extern fault_handler]

isr_common_handler:

  ; Save registers
  pusha
  push ds
  push es
  push fs
  push gs

  ; Load the data segment descriptor
  mov ax, 0x10 
  mov ds, ax
  mov es, ax
  mov fs, ax
  mov gs, ax

  push esp
  call fault_handler    
  
  pop eax
  pop gs
  pop fs
  pop es
  pop ds
  popa
  add esp, 8     ; Cleanup
  iret           ; Return from interrupt


PARAM_EBX equ 0x4
PARAM_ECX equ 0x8
PARAM_EDX equ 0xC
PARAM_ESI equ 0x10
PARAM_EDI equ 0x14

[extern create_file]
[global create_file_handler]
create_file_handler:
  
  push dword [esp + PARAM_EDX]
  push dword [esp + PARAM_ESI]
  push dword [esp + PARAM_EDI]
  
  call create_file 
  
  iret
