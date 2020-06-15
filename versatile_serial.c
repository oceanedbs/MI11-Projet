/*
 * serial.c
 *
 *  Created on: 24 mars 2020
 *      Author: mi11
 */


#include "versatile_serial.h"

#define UARTCLK (24000000)
#define UARTCR_RXE (1 << 9)
#define UARTCR_TXE (1 << 8)
#define UARTCR_UARTEN (1 << 0)
#define UARTLCR_H_WLEN_8 (3 << 5)
#define UARTLCR_H_FEN (1 << 4)
#define UARTFR_BUSY (1 << 3)
#define UARTFR_TXFF (1 << 5)
#define UARTFR_TXFE (1 << 7)
#define UARTFR_RXFE (1 << 4)
#define UARTFR_RXFF (1 << 6)

typedef volatile struct pl011_uart {
	uint32_t uartd;
	uint32_t uartresr;
	uint32_t reserved1[4];
	uint32_t uartfr;
	uint32_t uartilpr;
	uint32_t uartibrd;
	uint32_t uartfbrd;
	uint32_t uartlcr_h;
	uint32_t uartcr;
} pl011_uart_t;


pl011_uart_t* pl011_uarts[] = {
		(void *) 0x101f1000,
		(void *) 0x101f2000,
		(void *) 0x101f3000,
};

void serial_init(int nuart, int baudrate)
{
	int divisor_int;
	double divisor;
	double divisor_frac;

	pl011_uart_t* uart = pl011_uarts[nuart];

	divisor = ((double)(UARTCLK)) / (16.0 * baudrate);
	divisor_int = divisor;
	divisor_frac = divisor - (double)(divisor_int);

	uart->uartibrd = divisor_int;
	uart->uartfbrd = (int)(divisor_frac * 64.0 + 0.5);

	uart->uartcr &=~UARTCR_UARTEN;
	while (uart->uartfr & UARTFR_BUSY) continue;
	uart->uartlcr_h = UARTLCR_H_WLEN_8;
	uart->uartcr = UARTCR_RXE | UARTCR_TXE;
	uart->uartcr = UARTCR_UARTEN;
}

void serial_write(int nuart, char c)
{
	pl011_uart_t* uart = pl011_uarts[nuart];

	while (uart->uartfr & UARTFR_TXFF) continue;
	uart->uartd = c;
}

char serial_read(int nuart)
{
	pl011_uart_t* uart = pl011_uarts[nuart];
	while (!(uart->uartfr & UARTFR_RXFF)) continue;

	return uart->uartd;
}
