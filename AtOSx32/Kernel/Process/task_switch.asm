[extern running_task]       ; Current task
[extern init_task]
[extern allow_ts]

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
  
   
  mov esi, dword [running_task]

  mov dword [allow_ts], 1

  ; Save CPU time
  mov eax, dword [esi+0x1C]
  mov dword [.init_task], eax

  ; Cdecl registers
  pop ebp
  pop edi
  pop esi
  pop ebx

  sti

  ; CPU time, if 0 we initialize the task
  or dword [.init_task], 0
  jz .wrapper

  ret     ; Normal task switching, popping off the last EIP

.wrapper:
  
  call init_task
  jmp $

.init_task dd 0
