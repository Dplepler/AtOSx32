[extern running_task]       ; Current task
[extern make_thread]

[global switch_task]


; void switch_task(struct _PROCESS_CONTROL_BLOCK_STRUCT* new_task)
switch_task:

  ; Cdecl registers
  push ebx
  push esi
  push edi
  push ebp 

  mov esi, dword [running_task]

  mov dword [esi+0x8], esp    ; Set previous task's esp
  
  mov esi, dword [esp+0x14]  ; Get new task
  mov esp, dword [esi+0x8]   ; ESP

  mov dword [running_task], esi   ; running_task = new task
  
  mov eax, dword [esi+0x10]  ; CR3 (Physical address space)
  mov edx, cr3

  cmp eax, edx
  je .continue

  mov cr3, eax  ; Change address space accordingly

.continue:
  
  ; Cdecl registers
  pop ebp
  pop edi
  pop esi
  pop ebx
   
  mov esi, dword [running_task]

  sti

  or dword [esi+0x1C], 0    ; CPU time, if 0 we initialize the task
  jz .wrapper

  ret     ; Normal task switching, popping off the last EIP

.wrapper:
  
  call make_thread
  jmp $

