[extern running_task]       ; Current task
[extern task_state]         ; TSS

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
  mov ecx, dword [esi+0x8]   ; ESP
  mov eax, dword [esi+0x4]   ; ESP0

  mov edx, eax
  sub edx, ecx    ; Variables in stack (times 4)
  
  sub eax, 0x1000    ; Literal start of stack
  call calc_phys      

  or eax, 3                 ; Read+Write and present flags set
  mov edi, 0xFFF00000       ; Write to kernel
  mov dword [edi+0x3FE*0x4], eax  ; ESP0

  mov esp, 0xC03FE000
  add esp, 0x1000
  sub esp, edx

  mov dword [running_task], esi   ; Current task = new task
  mov eax, dword [esi+0x4]        ; Get new task's kernel stack
  
  ; Set the tss's esp0 accordingly
  lea edi, [task_state]
  mov dword [edi+0x4], eax
 
  mov eax, dword [esi+0x10]  ; CR3 (Address space)
  mov edx, cr3
  
  cmp eax, edx
  je .continue

  mov cr3, eax  ; Change address space accordingly
  

.continue:
  
  pop ebp
  pop edi
  pop esi
  pop ebx
  
  
  ret


calc_phys:
  
  push ecx
  push edx
  push esi
  push edi

  mov ecx, eax
  mov edx, eax
  
  shr ecx, 22     ; PD index
  shr edx, 12     ; PT index
  and edx, 0x3FF

  push eax    ; Address
  
  ; Calculate page table address 
  mov esi, 0xFFC00000 
  shl ecx, 12
  add esi, ecx
  
  mov eax, dword [esi+edx*0x4]  ; Physical address + flags
  
  and eax, 0xFFFFF000           ; Remove flags

  pop edx
  and edx, 0xFFF                 ; In-page offset

  add eax, edx

  pop edi
  pop esi
  pop edx
  pop ecx

  ret
  
