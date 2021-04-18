.macosx_version_min 10, 10 sdk_version 11, 3

.globl _main

.section __TEXT,__text
 _main:
  pushq %rbp
  movq %rsp, %rbp
  leaq str(%rip), %rdi
  callq _puts
  xorl %eax, %eax
  popq %rbp
  retq

.section __DATA,__data
  str: .asciz "Hello, World!"
