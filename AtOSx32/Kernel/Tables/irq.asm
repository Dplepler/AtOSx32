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


