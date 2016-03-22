#include "spark_wiring_usbserial.h"

#ifdef __cplusplus
extern "C" {
#endif

void serial_begin(unsigned long baudrate) {
	Serial.begin(baudrate);
}

int serial_available()
{
    return Serial.available();    
}

unsigned char serial_read()
{
    return Serial.read();    
}

void serial_putc(unsigned char c)
{
    Serial.write(c);
}

#ifdef __cplusplus
}
#endif
