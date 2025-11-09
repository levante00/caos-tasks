.intel_syntax noprefix

.text
.global add_scanf
.extern scanf

add_scanf:
  push rbp
  mov rbp, rsp 
  sub rsp, 16

  lea rdi, scanf_format_string
  lea rsi, [rbp - 8]
  lea rdx, [rbp - 16]

  call scanf

  mov rax, [rbp - 8]
  mov rdx, [rbp - 16]
  add rax, rdx

  mov rsp, rbp
  pop rbp
  ret

  .section .rodata

scanf_format_string:
  .string "%lld %lld"
