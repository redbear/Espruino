#ifndef _UARTSERIAL_API_H
#define _UARTSERIAL_API_H


void serial_begin(unsigned long baudrate);
int serial_available();
unsigned char serial_read();
void serial_putc();

#endif
