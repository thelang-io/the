global _main

section .text
 _main:
  mov rax, 0x02000004
  mov rdi, 1
  mov rsi, message
  mov rdx, 14
  syscall

  mov eax, 0x2000001
  mov edi, 0
  syscall

section .data
  message: db "Hello, World!", 10
