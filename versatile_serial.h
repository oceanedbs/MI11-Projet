/*
 * uart.h
 *
 *  Created on: 24 mars 2020
 *      Author: mi11
 */

#ifndef VERSATILE_SERIAL_H_
#define VERSATILE_SERIAL_H_

#include <stdint.h>

void serial_init(int uart, int baudrate);
void serial_write(int uart, char c);
char serial_read(int uart);


#endif /* VERSATILE_SERIAL_H_ */
