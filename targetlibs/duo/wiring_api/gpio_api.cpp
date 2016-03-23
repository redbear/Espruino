#include "spark_wiring.h"

#ifdef __cplusplus
extern "C" {
#endif


void gpio_pinMode(uint16_t pin, PinMode mode)
{
	pinMode(pin, mode);
}

PinMode gpio_getPinMode(uint16_t pin)
{
	return getPinMode(pin);
}

bool gpio_pinAvailable(uint16_t pin)
{
    return pinAvailable(pin);
}

void gpio_setADCSampleTime(uint8_t ADC_SampleTime)
{
	setADCSampleTime(ADC_SampleTime);
}

int32_t gpio_analogRead(uint16_t pin)
{
    return analogRead(pin);
}

void gpio_analogWrite(uint16_t pin, uint16_t value)
{
	analogWrite(pin, value);
}

void gpio_digitalWrite(uint16_t pin, uint8_t value)
{
	digitalWrite(pin, value);
}

int32_t gpio_digitalRead(uint16_t pin)
{
    return digitalRead(pin);
}

long gpio_map(long value, long fromStart, long fromEnd, long toStart, long toEnd)
{
    return map(value, fromStart, fromEnd, toStart, toEnd);
}

void gpio_shiftOut(uint8_t dataPin, uint8_t clockPin, uint8_t bitOrder, uint8_t val)
{
	shiftOut(dataPin, clockPin, bitOrder, val);
}

uint8_t gpio_shiftIn(uint8_t dataPin, uint8_t clockPin, uint8_t bitOrder)
{
    return shiftIn(dataPin, clockPin, bitOrder);
}

uint32_t gpio_pulseIn(pin_t pin, uint16_t value)
{
     return pulseIn(pin, value);
}


#ifdef __cplusplus
}
#endif
