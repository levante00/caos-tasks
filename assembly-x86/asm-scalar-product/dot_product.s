  .intel_syntax noprefix

  .text
  .global dot_product


  # rdi -> size_t N
  # rsi - const float * A
  # rdx - const float * B
  # return value should be in xmm0

dot_product:
  push rbp
  mov rbp, rsp 
  xorps xmm0, xmm0

loop_1:
  cmp rdi, 4
  jl loop_2

  movups xmm1, [rsi]
  movups xmm2, [rdx]
  mulps xmm1, xmm2
  addps xmm0, xmm1

  add rsi, 16
  add rdx, 16
  sub rdi, 4    
  jmp loop_1

loop_2:
  test rdi, rdi
  jz end    
  
  xorps xmm1, xmm1
  xorps xmm2, xmm2

  movups xmm1, [rsi]
  movups xmm2, [rdx]
  mulps xmm1, xmm2
  addps xmm0, xmm1

end: 
  pop rbp
  haddps xmm0, xmm0
  haddps xmm0, xmm0
  
  ret
