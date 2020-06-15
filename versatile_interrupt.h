/*
 * versatile_interrupt.h
 *
 *  Created on: 31 mars 2020
 *      Author: mi11
 */

#ifndef VERSATILE_INTERRUPT_H_
#define VERSATILE_INTERRUPT_H_

#define _irq_enable_() 			\
asm volatile(					\
	"mrs	r0, cpsr\t\n"		\
	"bic	r0, r0, #0x80\t\n"	\
	"msr	cpsr_c, r0\t\n"		\
	:							\
	:							\
	: "r0")

#define _irq_disable_() 			\
asm volatile(					\
	"mrs	r0, cpsr\t\n"		\
	"orr	r0, r0, #0x80\t\n"	\
	"msr	cpsr_c, r0\t\n"		\
	:							\
	:							\
	: "r0")


void interrupt_enable(int n);

void interrupt_disable(int n);


#endif /* VERSATILE_INTERRUPT_H_ */
