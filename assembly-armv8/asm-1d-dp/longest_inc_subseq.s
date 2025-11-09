.text
.global longest_inc_subseq

.macro push Xn
  sub sp, sp, 8
  str \Xn, [sp]
.endm

.macro pop Xn
  ldr \Xn, [sp]
  add sp, sp, 8
.endm

# x0 -> int64_t* array, (values array)
# x1 -> uint64_t* help_array, (dynamic array)
# x2 -> uint64_t size, (array's size)
# x3 -> i, (loop index 1)
# x4 -> j, (loop index 2)
# x5 -> 1 -> temp asnwer
# x6 -> array[i]
# x7 -> array[j]
# x8 -> help_array[i]  
# x9 -> 1 + help_array[j]  

longest_inc_subseq:
  push x30
  push x29
  mov x29, sp
  mov x5, 0

  cmp x2, xzr 
  beq end

  mov x3, 0 
  mov x5, 1

  loop_1: 
    str x5, [x1, x3, LSL 3]
    mov x4, 0

    loop_2:
      ldr x6, [x0, x3, LSL 3]
      ldr x7, [x0, x4, LSL 3]
      cmp x6, x7
      ble next_1
      
      ldr x8, [x1, x3, LSL 3]
      ldr x9, [x1, x4, LSL 3]
      add x9, x9, 1
      cmp x8, x9
      bhi next_1
      str x9, [x1, x3, LSL 3]

      next_1:
        add x4, x4, 1
        cmp x4, x3
        blt loop_2

    add x3, x3, 1
    cmp x3, x2
    blt loop_1

  ldr x5, [x1]
  mov x3, 0 
  loop_3: 
    ldr x8, [x1, x3, LSL 3]
    cmp x5, x8 
    bhi next_3
    mov x5, x8 
    next_3:
      add x3, x3, 1
      cmp x3, x2
      blt loop_3


  end:
    mov x0, x5
    mov sp, x29
    pop x29
    pop x30
  ret
