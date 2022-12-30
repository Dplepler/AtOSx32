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
  
  ret
