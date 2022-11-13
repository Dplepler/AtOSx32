[extern running_task]       ; Current task
[extern task_state] ; TSS

[global switch_task]


; void switch_task(struct _PROCESS_CONTROL_BLOCK_STRUCT* new_task)
switch_task: 

  push ebx
  push esi
  push edi
  push ebp
 
  mov esi, dword [running_task]
  mov dword [esi+0x8], esp    ; Set previous task's esp
  
  mov esi, dword [esp+0x14]  ; Get new task
  mov esp, dword [esi+0x8]   ; Set esp

  mov dword [running_task], esi     ; Current task = new task
  mov eax, dword [esi+0x4]  ; Get new task's kernel stack

  ; Set the tss's esp0 accordingly
  lea edi, [task_state]
  mov dword [edi+0x4], eax

  mov eax, dword [esi+0x10]  ; CR3 (Address space)
  mov edx, cr3

  cmp eax, edx
  je .continue
 
  ;mov cr3, eax  ; Change address space accordingly
    
.continue:

  pop ebp
  pop edi
  pop esi
  pop ebx
  
  ret

