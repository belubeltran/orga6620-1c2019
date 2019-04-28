#ifndef USE_MIPS_ASSEMBLY
#define USE_MIPS_ASSEMBLY
#include <sys/regdef.h>

#define SS 8
#define O_FP 4
#define O_GP 0
#define O_A0 8
#define O_A1 12

.text
.align 2
.globl getNextOrientation

getNextOrientation:
	subu sp, sp, SS # SRA = 8, LTA = 0, ABA = 0
	sw fp, O_FP(sp)
	sw gp, O_GP(sp)
	move fp, sp
	sw a0, O_A0(fp) # Saving parameters 
	sw a1, O_A1(fp)

main:
	beqz a0, is_north # current orientation = north
	beq a0, 1, is_south # current orientation = south
	beq a0, 2, is_east # current orientation = east

is_west: # current orientation = west
	beqz a1, go_south # rotate left

go_north:
	addiu v0, 0, 0 # new orientation = north
	b exit

go_south:
	addiu v0, 0, 1 # new orientation = south
	b exit

is_east: # current orientation = east
	beqz a1, go_south # rotate left
	b go_north # rotate right

is_north: # current orientation = north
	beqz a1, go_west # rotate left

go_east:
	addiu v0, 0, 2 # new orientation = east
	b exit

go_west:
	addiu v0, 0, 3 # new orientation = west
	b exit

is_south: # current orientation = south
	beqz a1, go_east # rotate left
	b go_west # rotate right

exit:
	lw a0, O_A0(sp)
	lw a1, O_A1(sp)
	lw fp, O_FP(sp)
	lw gp, O_GP(sp)
	addiu sp, sp, SS
	j ra

#endif