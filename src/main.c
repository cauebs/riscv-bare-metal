#include "uart.h"
#include "machine.h"

void print_hart_id() {
    unsigned int hart_id = mhartid();
    switch (hart_id) {
        case 1: print("1"); break;
        case 2: print("2"); break;
        case 3: print("3"); break;
        case 4: print("4"); break;
    }
}

void pong();

void ping() {
    print("hart ");
    print_hart_id();
    print(": ping\n");
    schedule_interrupt(mhartid(), 1000, pong);
}

void pong() {
    print("hart ");
    print_hart_id();
    print(": pong\n");
    schedule_interrupt(mhartid(), 1000, ping);
}

int main() {
    unsigned int hart_id = mhartid();
    unsigned long initial_delay = 250 * hart_id;
    schedule_interrupt(hart_id, initial_delay, ping);
}
