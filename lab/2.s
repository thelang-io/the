global _main

section .text
 _main:
  jmp printf

  mov rax, 0x02000001
  mov rdi, 0
  syscall

 printf:
  mov rax, 0x02000004
  mov rdi, 1
  mov rsi, message
  mov rdx, message.len
  syscall
  ret

section .data
  message: db "Hello, World!", 10
  .len: equ $ - message
