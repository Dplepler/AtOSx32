[extern task_state]
[extern running_task]
[global jmp_userland]

; jmp_userland(void* func)
jmp_userland:
   
  mov edx, dword [esp + 0x4] 
  
  mov esi, task_state
  mov edi, dword [running_task]
 
  ; Switch from kernel stack to user stack
  mov dword [esi + 0x4], esp
  mov esp, dword [edi + 0x8]

  push 0x0
  mov ax, 0x23
  mov ds, ax
  mov es, ax
  mov fs, ax
  mov gs, ax

  mov eax, esp 
  push 0x23    ; Data segment
  or eax, 0x200
  push eax    ; Stack
  pushf       ; Flags
  push 0x1b   ; Code segment
  push edx    ; EIP

  iret

