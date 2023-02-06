[global print]
print:

  mov ah, 1
  mov esi, dword [esp + 0x4]
  int 0x45

  ret

[global getchar]
getchar:

.loop:
  xor al, al
  mov ah, 2
  int 0x45
  
  and al, 0xFF
  jz .loop

  ret

[global malloc]
malloc:
  
  mov ah, 3
  mov edx, dword [esp + 0x4]
  int 0x45

  ret

[global realloc]
realloc:
  
  mov ah, 4
  mov edx, dword [esp + 0x8]
  mov esi, dword [esp + 0x4] 
  int 0x45

  ret

[global printh]
printh:

  mov ah, 5
  mov ebx, dword [esp + 0x4]
  int 0x45

  ret
