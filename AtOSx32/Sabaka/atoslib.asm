
[global print]
print:

  mov ah, 1
  mov esi, dword [esp + 0x4]
  int 0x45
