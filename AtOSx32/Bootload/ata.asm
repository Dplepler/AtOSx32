[bits 32]
; BL = Sector amount
; DI = Buffer
; ECX = LBA
ata_read_s:

  pusha
 
  call wait_bsy

  mov dx, 1F2h
  mov al, bl
  out dx, al
  
  mov al, cl
  mov dx, 1F3h
  out dx, al
  mov al, ch
  mov dx, 1F4h
  out dx, al
  shr ecx, 16
  mov al, cl
  mov dx, 1F5h
  out dx, al

  mov dx, 1F6h
  mov al, 40h
  out dx, al


  mov dx, 1F7h
  mov al, 20h
  out dx, al 
 
  xor ecx, ecx
  mov cl, bl

.read:

  call wait_bsy
  call wait_drq
  
  push cx
  mov dx, 1F0h
  mov cx, 256

  rep insw 
  pop cx
  call delay
  
  loop .read
 
  popa
  ret

wait_bsy:

  mov dx, 1F7h
  in al, dx
  and al, 80h
  jnz wait_bsy
  ret

wait_drq:

  mov dx, 1F7h
  in al, dx
  and al, 40h
  jz wait_drq
  ret

delay:

  mov dx, 3F6h
  in al, dx
  in al, dx 
  in al, dx 
  in al, dx 
  ret

