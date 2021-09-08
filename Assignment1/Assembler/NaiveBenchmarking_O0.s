	.file	"NaiveBenchmarking.c"
	.text
	.section	.rodata
	.align 8
.LC1:
	.string	"The sum is %lu for size %lu \nand the time it took is %f seconds\n"
	.text
	.globl	main
	.type	main, @function
main:
.LFB0:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$48, %rsp
	movq	$1000000000, -32(%rbp)
	movq	$0, -48(%rbp)
	call	clock@PLT
	movq	%rax, -24(%rbp)
	movq	$1, -40(%rbp)
	jmp	.L2
.L3:
	movq	-40(%rbp), %rax
	addq	%rax, -48(%rbp)
	addq	$1, -40(%rbp)
.L2:
	movq	-40(%rbp), %rax
	cmpq	-32(%rbp), %rax
	jbe	.L3
	call	clock@PLT
	movq	%rax, -16(%rbp)
	movq	-16(%rbp), %rax
	subq	-24(%rbp), %rax
	cvtsi2sdq	%rax, %xmm0
	movsd	.LC0(%rip), %xmm1
	divsd	%xmm1, %xmm0
	movsd	%xmm0, -8(%rbp)
	movq	-8(%rbp), %rcx
	movq	-32(%rbp), %rdx
	movq	-48(%rbp), %rax
	movq	%rcx, %xmm0
	movq	%rax, %rsi
	leaq	.LC1(%rip), %rdi
	movl	$1, %eax
	call	printf@PLT
	movl	$0, %eax
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE0:
	.size	main, .-main
	.section	.rodata
	.align 8
.LC0:
	.long	0
	.long	1093567616
	.ident	"GCC: (Ubuntu 9.3.0-17ubuntu1~20.04) 9.3.0"
	.section	.note.GNU-stack,"",@progbits
	.section	.note.gnu.property,"a"
	.align 8
	.long	 1f - 0f
	.long	 4f - 1f
	.long	 5
0:
	.string	 "GNU"
1:
	.align 8
	.long	 0xc0000002
	.long	 3f - 2f
2:
	.long	 0x3
3:
	.align 8
4:
