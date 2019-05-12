#ifndef USE_MIPS_ASSEMBLY
#define USE_MIPS_ASSEMBLY
#include <sys/regdef.h>

#define SS 8
#define O_FP 4
#define O_GP 0
#define O_A0 8
#define O_A1 12
#define O_A2 16
#define O_ANTX 0
#define O_ANTY 4
#define O_ANTO 8

.text
.align 2
.globl advanceAnt

advanceAnt:
	subu sp, sp, SS # SRA = 8, LTA = 0, ABA = 0
	sw fp, O_FP(sp)
	sw gp, O_GP(sp)
	move fp, sp
	# Saving parameters
	sw a0, O_A0(fp) # ant* 
	sw a1, O_A1(fp) # width
	sw a2, O_A2(fp) # height

main:
	lw t0, O_ANTO(a0) # t0 = ant->o (orientation)
	lw t1, O_ANTX(a0) # t1 = ant->x
	lw t2, O_ANTY(a0) # t2 = ant->y
	beqz t0, is_north
	beq t0, 1, is_south
	beq t0, 2, is_east

is_west:
	bgtz t1, decrement_x
	subu t3, a1, 1 # t3 = width - 1
	b save_new_x

decrement_x:
	subu t3, t1, 1 # t3 = ant->x - 1

save_new_x:
	sw t3, 0(a0) # save new ant->x
	b exit

is_north:
	bgtz t2, decrement_y
	subu t3, a2, 1 # t3 = height - 1
	b save_new_y

decrement_y:
	subu t3, t2, 1 # t3 = ant->y - 1

save_new_y:
	sw t3, 4(a0) # save new ant->y
	b exit

is_south:
	subu t4, a2, 1 # t4 = height - 1
	blt t2, t4, increment_y
	addiu t3, zero, 0 # t3 = ant->y = 0
	b save_new_y

increment_y:
	addiu t3, t2, 1 # t3 = ant->y + 1
	b save_new_y

is_east:
	subu t4, a1, 1 # t4 = width - 1
	blt t1, t4, increment_x
	addiu t3, zero, 0 # t3 = ant->x = 0
	b save_new_x

increment_x:
	addiu t3, t1, 1 # t3 = ant->x + 1
	b save_new_x

exit:
	lw a0, O_A0(sp)
	lw a1, O_A1(sp)
	lw a2, O_A2(sp)
	lw fp, O_FP(sp)
	lw gp, O_GP(sp)
	addiu sp, sp, SS
	j ra

#endif