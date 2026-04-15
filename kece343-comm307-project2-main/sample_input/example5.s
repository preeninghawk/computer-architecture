	.data
data1:	.word	3
	.word	4096
	.word	0xf4c2a
data2:	.word	2147483646
data3:	.word	2147483647
	.text
main:
	la	x15, data2
	addi	x12, x12, -100
	addi	x14, x14, -1
	addi	x13, x13, 0xffff
	add	x2, x4, x5
	add	x2, x6, x7
	sub	x9, x3, x2
cs311:
	and	x11, x11, x0
	addi	x10, x10, 0x1
	or	x6, x6, x0
	jal	x31, cs312
cs312:
	lw	x6, 4(x15)
	slli	x7, x6, 2
	sw	x7, 8(x15)
	srli	x5, x4, 2
	or	x28, x5, x0
	xori	x5, x28, -1
	sltiu	x9, x10, -4
	beq	x9, x0, cs313
	jalr	x30, 0(x31)
cs313:
	lui	x17, 0x1000
	lw	x18, 0(x17)
	sltu	x4, x2, x3
	bne	x4, x0, cs314
	jal	x29, cs311
cs314:
	ori	x16, x16, 0xf0f0
