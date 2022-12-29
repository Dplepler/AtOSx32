global jmp_userland

; jmp_userland(void* func)
; Input: Function destination in ESI
jmp_userland:
  
  cli
  mov esi, dword [esp + 0x4] 
  
  mov ax, 0x20
  or ax, 3

  mov ds, ax
  mov es, ax
  mov fs, ax
  mov gs, ax

  push eax
  push esp
  pushfd
  or dword [esp], 0x200
  mov eax, 0x18
  or eax, 3
  push eax

  push esi

  iret

