#include "uart.h"
#include "machine.h"

extern void trap_handler(unsigned long* regs, unsigned long mcause, unsigned long mepc) {
	unsigned long interrupt_bit = 1UL << 63;
	unsigned is_interrupt = (mcause & interrupt_bit) != 0;
	
	if (!is_interrupt) {
		print("unhandled trap!\n");
		return;
	}

	unsigned int exception_code = mcause & 0b1111111111;

	if (exception_code == 7) {
		print("machine timer interrupt hit for hart ");
		switch (mhartid()) {
			case 1: print("1"); break;
			case 2: print("2"); break;
			case 3: print("3"); break;
			case 4: print("4"); break;
		}
		print("\n");
		schedule_interrupt(1000);
	}
}

int main() {
	enable_machine_timer_interrupt();

	unsigned long initial_delay = 250 * mhartid();
	schedule_interrupt(initial_delay);
}
