[extern task]   ; Current task
[extern task_state]

[global switch_task]


; void switch_task(struct _PROCESS_CONTROL_BLOCK_STRUCT* new_task)
switch_task:
  
  push esi

  mov esi, dword [task]
  mov dword [esi+0x4], esp  ; Set previous task's esp0
  
  mov esi, dword [esp+0x8]  ; Get new task

  mov esp, dword [esi+0x8]  ; Set esp
  mov eax, dword [esi+0xC]  ; CR3 (Address space)
  mov edx, cr3

  cmp eax, edx
  je .continue
  
  mov cr3, eax  ; Change address space accordingly

.continue:

  mov eax, dword [esi+0x4]    ; Get new task's esp0
  ; Set the tss' es0 accordingly
  lea esi, [task_state] 
  mov dword [esi+0x4], eax

  pop esi

  ret

