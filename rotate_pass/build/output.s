	.text
	.file	"test.c"
	.globl	main                            # -- Begin function main
	.p2align	4
	.type	main,@function
main:                                   # @main
	.cfi_startproc
# %bb.0:
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	$0, -12(%rbp)
	movl	$0, -8(%rbp)
	movl	$0, -4(%rbp)
	xorl	%eax, %eax
	testb	%al, %al
	jne	.LBB0_2
	.p2align	4
.LBB0_1:                                # =>This Inner Loop Header: Depth=1
	movl	-4(%rbp), %eax
	addl	%eax, -8(%rbp)
	incl	%eax
	movl	%eax, -4(%rbp)
	cmpl	$4, %eax
	jl	.LBB0_1
.LBB0_2:
	movl	-12(%rbp), %eax
	popq	%rbp
	.cfi_def_cfa %rsp, 8
	retq
.Lfunc_end0:
	.size	main, .Lfunc_end0-main
	.cfi_endproc
                                        # -- End function
	.ident	"clang version 20.0.0git (https://github.com/llvm/llvm-project.git 916bae2d921705c8ce78a4ddec4503c61bc8220c)"
	.section	".note.GNU-stack","",@progbits
