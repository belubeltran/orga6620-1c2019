#ifndef USE_MIPS_ASSEMBLY
#define USE_MIPS_ASSEMBLY
#include <mips/regdef.h>
#include <sys/syscall.h>

#define SS 56
#define O_RA 48
#define O_FP 44
#define O_GP 40
#define O_P 16
#define O_R 20
#define O_G 24
#define O_A 28
#define O_I 32
#define O_A0 56
#define O_A1 60
#define O_A2 64
#define O_A3 68

.text
.align 2
.globl paint
.extern advanceAnt
.extern getOrientation

paint:
	subu sp, sp, SS # SRA = 16, LTA = 24, ABA = 16
	sw ra, O_RA(sp)
	sw $fp, O_FP(sp)
	sw gp, O_GP(sp)
	move $fp, sp
	# Saving parameters
	sw a0, O_A0($fp) # *ant
	sw a1, O_A1($fp) # *grid
	sw a2, O_A2($fp) # *palette
	sw a3, O_A3($fp) # *rules
	# what do I do with the 5th param?

main:
	addiu t0, a2, 1 # t0 = *palette + 1
	sw t0, O_P($fp) # Saving *p in LTA
	addiu t1, 0, 0 # i = 0
	sw t1, O_I($fp) # Saving i in LTA

for:
	lw t2, 0(a0) # t2 = ant->x
	lw t3, 4(a0) # t3 = ant->y
	lw t4, 8(a1) # t4 = *grid
	addiu t4, t4, t3 # moving the pointer in the y direction of the grid
	addiu t4, t4, a1 # getting the dir
	lw t5, 0(t4) # getting the height array dir
	addiut t5, t5, t2 # moving the pointer in the x dir of the grid
	lw t4, O_P($fp) # Cargo *p
	sw t4, 0(t5) # g->grid[a->y][a->x] = *p

exit:
	lw a0, O_A0(sp)
	lw a1, O_A1(sp)
	lw a2, O_A2(sp)
	lw a3, O_A3(sp)
	lw ra, O_RA(sp)
	lw $fp, O_FP(sp)
	lw gp, O_GP(sp)
	addiu sp, sp, SS
	j ra

#endif