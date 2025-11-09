.intel_syntax noprefix

.text
.global add

add:
  push rbp
  mov  rbp, rsp 
  
  mov rax, rdi
  add rax, rsi

  mov rsp, rbp
  pop rbp
  ret

