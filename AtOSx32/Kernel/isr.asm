[bits 32]

%macro ISR_NO_ERROR 1

  push byte 0
  push byte %1
  jmp isr_common_handler

%endmacro

%macro ISR_ERROR 1
 
  push byte %1
  jmp isr_common_handler

%endmacro

extern fault_handler

global isr0
isr0: ISR_NO_ERROR 0
global isr1
isr1: ISR_NO_ERROR 1
global isr2
isr2: ISR_NO_ERROR 2
global isr3
isr3: ISR_NO_ERROR 3
global isr4
isr4: ISR_NO_ERROR 4
global isr5
isr5: ISR_NO_ERROR 5
global isr6
isr6: ISR_NO_ERROR 6
global isr7
isr7: ISR_NO_ERROR 7
global isr8
isr8: ISR_ERROR 8
global isr9
isr9: ISR_NO_ERROR 9
global isr10
isr10: ISR_ERROR 10
global isr11
isr11: ISR_ERROR 11
global isr12
isr12: ISR_ERROR 12 
global isr13
isr13: ISR_ERROR 13 
global isr14
isr14: ISR_ERROR 14
global isr15
isr15: ISR_NO_ERROR 15
global isr16
isr16: ISR_NO_ERROR 16
global isr17
isr17: ISR_ERROR 17
global isr18
isr18: ISR_NO_ERROR 18
global isr19
isr19: ISR_NO_ERROR 19
global isr20
isr20: ISR_NO_ERROR 20
global isr21
isr21: ISR_NO_ERROR 21
global isr22
isr22: ISR_NO_ERROR 22
global isr23
isr23: ISR_NO_ERROR 23
global isr24 
isr24: ISR_NO_ERROR 24
global isr25
isr25: ISR_NO_ERROR 25
global isr26
isr26: ISR_NO_ERROR 26
global isr27
isr27: ISR_NO_ERROR 27
global isr28
isr28: ISR_NO_ERROR 28
global isr29
isr29: ISR_NO_ERROR 29
global isr30
isr30: ISR_NO_ERROR 30
global isr31
isr31: ISR_NO_ERROR 31

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
  
  mov eax, fault_handler
  call eax      
  pop eax
  pop gs
  pop fs
  pop es
  pop ds
  popa
  add esp, 8     ; Cleanup
  iret           ; Return from interrupt




