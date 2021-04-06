global _main

section .text
 printf:
  mov rax, 0x02000004
  mov rdi, 1
  mov rsi, message
  mov rdx, message.len
  syscall
  ret

 _main:
  call printf

  mov rax, 0x02000001
  mov rdi, 0
  syscall

section .data
  message: db "Hello, World!", 10
  .len: equ $ - message
