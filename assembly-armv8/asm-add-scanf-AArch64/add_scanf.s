.text
.global add_scanf
.extern scanf

.macro push Xn
  sub sp, sp, 8
  str \Xn, [sp]
.endm

.macro pop Xn
  ldr \Xn, [sp]
  add sp, sp, 8
.endm

add_scanf:
  push x30
  push x29
  mov x29, sp

  ldr x0, =scanf_format_string
  sub x1, sp, 8
  sub x2, sp, 16

  bl scanf

  ldr x0, [sp, -8]
  ldr x1, [sp, -16]
  add x0, x0, x1

  mov sp, x29
  pop x29
  pop x30
  ret 

  .section .rodata

scanf_format_string:
  .string "%lld %lld"
