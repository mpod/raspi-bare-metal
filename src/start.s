.section .init
.globl _start
.comm stack, 0x10000 @ Reserve 64k stack in the BSS
_start:

ldr sp, =stack+0x10000 @ Set up the stack
b main

/*
* This command tells the assembler to put this code with the rest.
*/
.section .text

