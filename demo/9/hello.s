	.file	"hello.c"
	.text
	.section	.rodata
.LC0:
	.string	"hello.c"
	.align 8
.LC1:
	.string	"(char *)main + 0x12 + offset == (char *)hello"
	.align 8
.LC2:
	.string	">>>=== *main = %p    *hello = %p offset = %d\n"
	.text
	.globl	hello
	.type	hello, @function
hello:
.LFB0:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$16, %rsp
	movq	main@GOTPCREL(%rip), %rax
	leaq	14(%rax), %rax
	movq	%rax, -8(%rbp)
	movq	-8(%rbp), %rax
	movl	(%rax), %eax
	movl	%eax, -12(%rbp)
	movl	-12(%rbp), %eax
	cltq
	leaq	18(%rax), %rdx
	movq	main@GOTPCREL(%rip), %rax
	addq	%rax, %rdx
	leaq	hello(%rip), %rax
	cmpq	%rax, %rdx
	je	.L2
	leaq	__PRETTY_FUNCTION__.0(%rip), %rax
	movq	%rax, %rcx
	movl	$11, %edx
	leaq	.LC0(%rip), %rax
	movq	%rax, %rsi
	leaq	.LC1(%rip), %rax
	movq	%rax, %rdi
	call	__assert_fail@PLT
.L2:
	movl	-12(%rbp), %eax
	movl	%eax, %ecx
	leaq	hello(%rip), %rax
	movq	%rax, %rdx
	movq	main@GOTPCREL(%rip), %rax
	movq	%rax, %rsi
	leaq	.LC2(%rip), %rax
	movq	%rax, %rdi
	movl	$0, %eax
	call	printf@PLT
	nop
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE0:
	.size	hello, .-hello
	.section	.rodata
	.type	__PRETTY_FUNCTION__.0, @object
	.size	__PRETTY_FUNCTION__.0, 6
__PRETTY_FUNCTION__.0:
	.string	"hello"
	.ident	"GCC: (Ubuntu 11.3.0-1ubuntu1~22.04) 11.3.0"
	.section	.note.GNU-stack,"",@progbits
	.section	.note.gnu.property,"a"
	.align 8
	.long	1f - 0f
	.long	4f - 1f
	.long	5
0:
	.string	"GNU"
1:
	.align 8
	.long	0xc0000002
	.long	3f - 2f
2:
	.long	0x3
3:
	.align 8
4:
