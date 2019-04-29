#ifndef USE_MIPS_ASSEMBLY
#define USE_MIPS_ASSEMBLY
#include <sys/regdef.h>
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
#define O_A4 72
#define O_ANTX 0
#define O_ANTY 4
#define O_ANTO 8
#define O_GRIDW 0
#define O_GRIDH 4
#define O_GRID 8

.text
.align 2
.globl paint
.extern advanceAnt
.extern getNextOrientation

paint:
	.ent paint
	.frame sp, SS, ra
  .set    noreorder
  .cpload t9
  .set    reorder
	subu sp, sp, SS # SRA = 16, LTA = 24, ABA = 16
	.cprestore O_GP
	# Saving pointers
	sw ra, O_RA(sp)
	sw fp, O_FP(sp)
	sw gp, O_GP(sp)
	move fp, sp
	# Saving parameters
	sw a0, O_A0(fp) # *ant
	sw a1, O_A1(fp) # *grid
	sw a2, O_A2(fp) # *palette
	sw a3, O_A3(fp) # *rules
	# what do I do with the 5th param?

main:
	addiu t0, a2, 1 # t0 = *palette + 1
	sw t0, O_P(fp) # Saving *p in LTA
	sw a3, O_R(fp) # Saving *r in LTA
	sw a1, O_G(fp) # Saving *g in LTA
	sw a0, O_A(fp) # Saving *a in LTA
	addiu t1, zero, 0 # i = 0
	sw t1, O_I(fp) # Saving i in LTA

for:
	lw t2, O_A4(fp) # Load iterations
	lw t3, O_I(fp) # Load current i
	bge t3, t2, exit # If (iterations < i) continue

	lw t0, O_A(fp) # Load a*
	l.s $f2, O_ANTX(t0) # t2 = ant->x
	l.s $f4, O_ANTY(t0) # t3 = ant->y
	l.d $f0, O_G(fp) # Load *g
	l.s $f6, O_GRID($f0) # Load *grid
	add.s $f6, $f6, $f4 # moving the pointer in the y direction of the grid
	l.s $f8, 0($f6) # getting the height array dir
	add.s $f8, $f8, $f2 # moving the pointer in the x dir of the grid
	l.s $f4, O_P(fp) # Loading *p
	s.s $f4, 0($f8) # g->grid[a->y][a->x] = *p

# 	lw t0, O_A(fp) # Load a*
# 	lw t1, O_ANTO(t0) # t0 = ant->o
# 	move a0, t1 # Save parameters for function call
# 	lw t0, O_I(fp) # Load current i
# 	lw t1, O_R(fp) # Load *r
# 	addu t0, t0, t1 # Getting dir
# 	lw t2, 0(t0) # Getting r[i]
# 	move a1, t2 # Save parameter for function call
# 	jal getNextOrientation
# 	lw t0, O_A(fp) # Load a*
# 	sw v0, O_ANTO(t0) # Saving new orientation

# 	lw t0, O_A(fp) # Load a*
# 	move a0, t0 # Saving parameters for function call
# 	lw t0, O_G(fp) # Load g*
# 	lw t1, O_GRIDW(t0) # Loading Grid width
# 	lw t2, O_GRIDH(t0) # Loading Grid height
# 	move a1, t1 # Saving parameters for function call
# 	move a2, t2 # Saving parameters for function call
# 	jal advanceAnt

# 	lw t0, O_P(fp) # Load *p
# 	addiu t0, t0, 1 # p++
# 	sw t0, O_P(fp) # Save new *p in LTA

# 	b for

exit:
	lw a0, O_A0(sp)
	lw a1, O_A1(sp)
	lw a2, O_A2(sp)
	lw a3, O_A3(sp)
	lw ra, O_RA(sp)
	lw fp, O_FP(sp)
	lw gp, O_GP(sp)
	addiu sp, sp, SS
	j ra
	.end paint

#endif