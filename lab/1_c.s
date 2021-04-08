.macosx_version_min 10, 10 sdk_version 11, 1
.globl _main

.section __TEXT, __text, regular, pure_instructions
  .p2align 4, 0x90

 _main:
  .cfi_startproc

  pushq %rbp
  .cfi_def_cfa_offset 16
  .cfi_offset %rbp, -16
  movq %rsp, %rbp
  .cfi_def_cfa_register %rbp
  leaq L_.str(%rip), %rdi
  callq _puts
  xorl %eax, %eax
  popq %rbp
  retq

  .cfi_endproc

.section __TEXT, __cstring, cstring_literals
  L_.str: .asciz "Hello, World!"
