#include <stdio.h>

unsigned long hart_id() {
	unsigned long id;
	asm("csrr %0, mhartid" : "=r"(id) : : );
	return id;
}

int main(int argc, char** argv) {
	printf("%ld\n", hart_id());
}
