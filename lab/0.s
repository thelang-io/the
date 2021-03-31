global _main

section .text
 _main:
  mov eax, 0x2000001
  mov edi, 0
  syscall
