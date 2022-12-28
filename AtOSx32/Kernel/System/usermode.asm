global jmp_userland

; jmp_userland(void* func)
; Input: Function destination in ESI
jmp_userland:
  
  mov esi, dword [esp + 0x4] 
  
  mov ax, 0x20
  or ax, 3

  mov ds, ax
  mov es, ax
  mov fs, ax
  mov gs, ax

  mov edx, esp
  push eax
  push edx
  pushf
  mov eax, 0x18
  or eax, 3
  push eax

  push esi

  iret

