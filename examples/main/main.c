#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "arch/riscv/trap.h"

volatile uintptr_t* mtime_address() {
	return (uintptr_t*) 0x0200BFF8UL;
}

volatile uintptr_t* mtimecmp_address(unsigned int hart_id) {
	return (uintptr_t*) (0x02004000UL + (8 * hart_id));
}

unsigned long read_mtime() {
	return *mtime_address();
}

unsigned long read_mtimecmp(unsigned int hart_id) {
	return *mtimecmp_address(hart_id);
}

void write_mtimecmp(unsigned int hart_id, uintptr_t time) {
	*mtimecmp_address(hart_id) = time;
}

static unsigned int read_mhartid() {
	unsigned int id;
	asm("csrr %0, mhartid" : "=r"(id) : : );
	return id;
}

static void set_mstatus(unsigned int bits) {
	asm("csrs mstatus, %0" : : "r"(bits) : );
}

static void set_mie(unsigned int bits) {
	asm("csrs mie, %0" : : "r"(bits) : );
}

void enable_machine_timer_interrupt() {
	unsigned int mie = 3;
	set_mstatus(1 << mie);
	
	unsigned int machine_timer = 7;
	set_mie(1 << machine_timer);
}

void schedule_interrupt(uintptr_t delay) {
	write_mtimecmp(read_mhartid(), read_mtime() + delay);
}

void my_handler(uintptr_t* regs, uintptr_t mcause, uintptr_t mepc) {
	uintptr_t interrupt_bit = 1UL << 63;
	unsigned is_interrupt = (mcause & interrupt_bit != 0);
	
	if (!is_interrupt) {
		printf("machine mode: unhandlable trap %d @ %p\n", mcause, mepc);
		exit(-1);
		return;
	}

	unsigned int exception_code = mcause & 0b1111111111;

	if (exception_code == 7) {
		printf(
			"machine timer interrupt hit for hart %ld at time %ld\n",
			read_mhartid(), read_mtime()
		);
		schedule_interrupt(1 << 20);
	}
}

int main(int argc, char** argv) {
	unsigned int hart_id = read_mhartid();
	printf("%ld\n", hart_id);
	
	if (hart_id == 0) {
		set_trap_fn(my_handler);
	}

	enable_machine_timer_interrupt();
	uintptr_t initial_delay = (1 << 20) / 5 * (hart_id + 1);
	schedule_interrupt(initial_delay);
}
