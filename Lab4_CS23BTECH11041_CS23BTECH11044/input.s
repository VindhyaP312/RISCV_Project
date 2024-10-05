.data
.dword 1000, 1001, 1002, 1003, 1004, 1005, 1006, 1007, 1008, 1009, 523, 524, 525, 533, 512
.text
lui x3, 0x10
lui x10, 0x10
addi x10, x10, 0x200
ld x4, 0(x3)
addi x3, x3, 8
Loop: beq x4, x0, Exit2
ld x5, 0(x3)
ld x6, 8(x3)
addi x3, x3, 16
gcd: beq x5, x0, Exit1
beq x6, x0, Exit1
beq x6, x5, Exit
bge x5, x6, Else
sub x6, x6, x5
beq x0, x0, Skip_Else
Else: sub x5, x5, x6
Skip_Else: beq x0, x0, gcd
Exit1: sd x0, 0(x10)
beq x0, x0, Loop_cont
Exit: sd x6, 0(x10)
Loop_cont: addi x10, x10, 8
addi x4, x4, -1
beq x0, x0, Loop
Exit2: add x0, x0, x0