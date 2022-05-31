#pragma once

enum {
    UART_REG_DIV    = 6,
    UART_REG_TXCTRL = 2,
    UART_TXEN       = 1,
    UART_REG_TXFIFO = 0,
};

static volatile int* uart = (int*) 0x10010000;

void uart_init() {
	unsigned int uart_freq = 32000000;
	unsigned int baud_rate = 115200;
    unsigned int divisor = uart_freq / baud_rate - 1;
    uart[UART_REG_DIV] = divisor;
    uart[UART_REG_TXCTRL] = UART_TXEN;
}

int uart_putchar(unsigned char ch) {
    while (uart[UART_REG_TXFIFO] < 0);
    return uart[UART_REG_TXFIFO] = ch & 0xff;
}

void print(char* s) {
	for (char* c = s; *c != '\0'; ++c) {
		uart_putchar(*c);
	}
}
