//taken from https://www.appelsiini.net/2011/simple-usart-with-avr-libc/
#ifndef UARTLIB_H
#define UARTLIB_H


#include <avr/io.h>
#include <stdio.h>

int uart_putchar(char c, FILE *stream);
int uart_getchar(FILE *stream);

void uart_init(void);

FILE uart_output = FDEV_SETUP_STREAM(uart_putchar, NULL, _FDEV_SETUP_WRITE);
FILE uart_input = FDEV_SETUP_STREAM(NULL, uart_getchar, _FDEV_SETUP_READ);

#endif