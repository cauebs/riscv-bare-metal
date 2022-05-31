.include "macros.s"
.include "constants.s"

.section .text.init,"ax",@progbits
.globl _start

_start:
    # disable interrupts
    csrci   mstatus, 3

    # setup default trap vector
    la      t0, trap_vector
    csrw    mtvec, t0

    # set up stack pointer based on hartid
    csrr    t0, mhartid
    slli    t1, t0, STACK_SHIFT
    la      sp, stacks + STACK_SIZE
    add     sp, sp, t1

    # harts 1, 2, 3, 4 skip initialization
    bnez    t0, _main
    
    lla     t0, _bss_start
    lla     t1, _bss_end
clear_bss:
    # monitor hart (0) zero-initializes memory and parks
    beq     t0, t1, park
    sd      zero, (t0)
    addi    t0, t0, 8
    j       clear_bss

_main:
    # re-enable interrupts
    csrci   mstatus, 3
    j       main

park:
    wfi
    j       park


.align 2
trap_vector:
    # Save registers.
    addi    sp, sp, -CONTEXT_SIZE
    sxsp    ra, 0
    sxsp    a0, 1
    sxsp    a1, 2
    sxsp    a2, 3
    sxsp    a3, 4
    sxsp    a4, 5
    sxsp    a5, 6
    sxsp    a6, 7
    sxsp    a7, 8
    sxsp    t0, 9
    sxsp    t1, 10
    sxsp    t2, 11
    sxsp    t3, 12
    sxsp    t4, 13
    sxsp    t5, 14
    sxsp    t6, 15

    # Invoke the handler.
    mv      a0, sp
    csrr    a1, mcause
    csrr    a2, mepc
    jal     trap_handler

    # Restore registers.
    lxsp    ra, 0
    lxsp    a0, 1
    lxsp    a1, 2
    lxsp    a2, 3
    lxsp    a3, 4
    lxsp    a4, 5
    lxsp    a5, 6
    lxsp    a6, 7
    lxsp    a7, 8
    lxsp    t0, 9
    lxsp    t1, 10
    lxsp    t2, 11
    lxsp    t3, 12
    lxsp    t4, 13
    lxsp    t5, 14
    lxsp    t6, 15
    addi sp, sp, CONTEXT_SIZE

    # Return
    mret


.bss
.align 4
.global stacks
stacks:
.skip STACK_SIZE * MAX_HARTS
