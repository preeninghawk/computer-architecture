	.data
	.text
main:
	sw	x10, 0(x2)
	sltiu	x5, x10, 1
	beq	x5, x0, L1
	addi	x2, x0, 1
L1:
	addi	x10, x10, -1
	jal	x1, main
	lw	x10, 0(x2)
	lw	x1, 4(x2)
	add	x2, x10, x2