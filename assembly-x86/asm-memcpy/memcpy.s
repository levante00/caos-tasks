  .intel_syntax noprefix

  .text
  .global my_memcpy
  
my_memcpy:
  push rbp
  mov rbp, rsp 
  sub rsp, 24
  mov [rbp - 8], rbx
  mov [rbp - 16], rcx

  # count == 0 
  test edx, edx
  jz end

  # rdi -> dest
  # rsi -> src 
  # rdx -> count

  # rax -> index
  # rbx -> variable for copying
  # rcx -> variable used for breaking from loop_1 if there are less then 8 bytes to copy

  mov rax, 0

loop_1:
  mov ecx, edx
  sub ecx, eax
  cmp ecx, 8 
  jle loop_2
  
  mov rbx, [rsi + rax]
  mov [rdi + rax], rbx
  add eax, 8
  cmp eax, edx 
  jle loop_1


loop_2:
  mov bl, [rsi + rax]
  mov [rdi + rax], bl
  add eax, 1
  cmp eax, edx
  jl loop_2

end: 
  mov rbx, [rbp - 8]
  mov rcx, [rbp - 16]
  mov rax, rdi
  mov rsp, rbp
  pop rbp
  ret
