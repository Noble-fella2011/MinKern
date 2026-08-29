.section .multiboot
.align 4
.long 0x1BADB002
.long 0x00000000
.long -(0x1BADB002 + 0x00000000)
.section .text
.global _start
.type _start, @function
_start:
    cli
    mov $0x3D4, %dx
    mov $0x0A, %al
    out %al, %dx
    mov $0x3D5, %dx
    mov $0x20, %al
    out %al, %dx
    mov $stack_top, %esp
    push %ebx
    push %eax
    call kernel_main
    cli
1:
    hlt
    jmp 1b
.section .bss
.align 16
stack_bottom:
.skip 16384
stack_top:
#how does this even work I barely understand this wtf
