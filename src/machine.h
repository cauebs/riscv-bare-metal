#pragma once

int main(void);

extern __attribute__((noreturn)) void _start_main() {
    main();
    while (1) {}
}

unsigned int mhartid() {
    unsigned int id;
    asm("csrr %0, mhartid" : "=r"(id) : : );
    return id;
}

volatile unsigned long* mtime() {
    return (unsigned long*) 0x0200BFF8UL;
}

volatile unsigned long* mtimecmp(unsigned int hart_id) {
    return (unsigned long*) (0x02004000UL + (8 * hart_id));
}

void set_mstatus(unsigned int bits) {
    asm("csrs mstatus, %0" : : "r"(bits) : );
}

void set_mie(unsigned int bits) {
    asm("csrs mie, %0" : : "r"(bits) : );
}

void enable_machine_timer_interrupt() {
    unsigned int mie = 3;
    set_mstatus(1 << mie);

    unsigned int machine_timer = 7;
    set_mie(1 << machine_timer);
}

const unsigned long MTIME_FREQ_HZ = 1000000;

typedef void (*_fn)();
static _fn _timer_interrupt_handler[4];

void schedule_interrupt(unsigned int hart_id, unsigned long milliseconds, void* handler) {
    _timer_interrupt_handler[hart_id] = handler;

    unsigned long delay = milliseconds * MTIME_FREQ_HZ / 1000;
    *mtimecmp(hart_id) = *mtime() + delay;

    enable_machine_timer_interrupt();
}

extern void trap_handler(unsigned long* regs, unsigned long mcause, unsigned long mepc) {
    unsigned long interrupt_bit = 1UL << 63;
    unsigned is_interrupt = (mcause & interrupt_bit) != 0;

    if (!is_interrupt) {
        print("unhandled trap!\n");
        return;
    }

    unsigned int exception_code = mcause & 0b1111111111;

    if (exception_code == 7) {
        _timer_interrupt_handler[mhartid()]();
    }
}
