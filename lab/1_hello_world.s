global print
global _main

section .data
  message: db "Hello, World!", 0x10

section .text
 _main:
  call print
  ret

 print:
  mov eax, 0x02000004
  mov edi, 0x01
  mov rsi, message
  mov edx, 0x0E
  syscall
  ret
