/*
 * interrupt.c
 *
 *  Created on: 31 mars 2020
 *      Author: mi11
 */


#include "versatile_interrupt.h"

#include <stdint.h>

typedef struct vic_tag {
	uint32_t irqstatus;
	uint32_t fiqstatus;
	uint32_t rawintr;
	uint32_t intselect;
	uint32_t intenable;
	uint32_t intenclear;
} vic_t;

#define VIC_BASE ((vic_t*)(0x10140000))

void interrupt_enable(int n)
{
	vic_t* vic = VIC_BASE;

	vic->intselect &= ~(1 << n);
	vic->intenable |= (1 << n);
}

void interrupt_disable(int n)
{
	vic_t* vic = VIC_BASE;

	vic->intenclear |= (1 << n);
}

int get_interrupt_number(void)
{
	vic_t* vic = VIC_BASE;

	int irqstatus = vic->irqstatus;

	for (int i = 0; i < 32; ++i) {
		if (irqstatus & 1) {
			return i;
		}

		irqstatus = irqstatus >> 1;
	}

	return -1;
}

