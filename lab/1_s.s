global _main

section .data
  message: db "Hello, World!", 10
  .len: equ $ - message

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
  ret
