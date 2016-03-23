#ifndef _GPIO_API_H
#define _GPIO_API_H

#include "pinmap_hal.h"

void     gpio_pinMode(uint16_t pin, PinMode mode);
PinMode  gpio_getPinMode(uint16_t pin);
bool     gpio_pinAvailable(uint16_t pin);

void     gpio_setADCSampleTime(uint8_t ADC_SampleTime);
int32_t  gpio_analogRead(uint16_t pin);
void     gpio_analogWrite(uint16_t pin, uint16_t value);

void     gpio_digitalWrite(uint16_t pin, uint8_t value);
int32_t  gpio_digitalRead(uint16_t pin);

long     gpio_map(long value, long fromStart, long fromEnd, long toStart, long toEnd);

void     gpio_shiftOut(uint8_t dataPin, uint8_t clockPin, uint8_t bitOrder, uint8_t val);
uint8_t  gpio_shiftIn(uint8_t dataPin, uint8_t clockPin, uint8_t bitOrder);

uint32_t gpio_pulseIn(pin_t pin, uint16_t value);


#endif
