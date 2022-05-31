#pragma once

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

void schedule_interrupt(unsigned long milliseconds) {
	unsigned long delay = milliseconds * MTIME_FREQ_HZ / 1000;
	*mtimecmp(mhartid()) = *mtime() + delay;
}
