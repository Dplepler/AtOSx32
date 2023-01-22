[global jmp_userland]

; jmp_userland(void* func)
jmp_userland:
  
  cli
  mov esi, dword [esp + 0x4] 
  
  mov ax, 0x20
  or ax, 3

  mov ds, ax
  mov es, ax
  mov fs, ax
  mov gs, ax

  mov eax, esp
  push 0x23
  push eax
  pushfd
  mov eax, 0x18
  or eax, 3
  
  push eax
  push esi

  iret

