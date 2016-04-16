/*
 * This file is part of Espruino, a JavaScript interpreter for Microcontrollers
 *
 * Copyright (C) 2013 Gordon Williams <gw@pur3.co.uk>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * ----------------------------------------------------------------------------
 * Platform Specific part of Hardware interface Layer
 * ----------------------------------------------------------------------------
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "jshardware.h"
#include "jsutils.h"
#include "jsparse.h"
#include "jsinteractive.h"
#include "jstimer.h"

#include "wiring.h"
#include "interrupts.h"


// see jshPinWatch/jshGetWatchedPinState
Pin watchedPins[16];

// ----------------------------------------------------------------------------
// for non-blocking IO
void jshInit() {
  jshInitDevices();
  
  int i;
  // reset some vars
  for (i=0;i<16;i++)
    watchedPins[i] = PIN_UNDEFINED;
  
  // start serial console for user interaction
  // for both USB and serial 1 of the Duo
  usartserial1_begin(9600);
  usbserial_begin(9600);  

  jshSetDeviceInitialised(EV_SERIAL1, true);
  jshSetDeviceInitialised(EV_SERIAL2, true);
}

void jshKill() {
}

void jshIdle() {
  // hack in order to get console device set correctly
  static bool inited = false;
  if (!inited) {
    inited = true;
    jsiOneSecondAfterStartup();
  }

  // read data from serial
  while (usartserial1_available())
    jshPushIOCharEvent(EV_SERIAL1, usartserial1_read());

  while (usbserial_available())
    jshPushIOCharEvent(EV_SERIAL1, usbserial_read());
}

// ----------------------------------------------------------------------------

int jshGetSerialNumber(unsigned char *data, int maxChars) {
  const char *code = "HelloWorld12";
  strncpy((char *)data, code, maxChars);
  return strlen(code);
}

// ----------------------------------------------------------------------------

static ALWAYS_INLINE uint8_t pinToEVEXTI(Pin ipin) {
  JsvPinInfoPin pin = pinInfo[ipin].pin;
  return (uint8_t)(EV_EXTI0+(pin-JSH_PIN0));
}

TIM_TypeDef* getTimerFromPinFunction(JshPinFunction device) {
  switch (device&JSH_MASK_TYPE) {
    case JSH_TIMER1:
      return TIM1;
    case JSH_TIMER2:
      return TIM2;
    case JSH_TIMER3:
      return TIM3;
    case JSH_TIMER4:
      return TIM4;
#ifndef STM32F3
    case JSH_TIMER5:
      return TIM5;
#endif
#ifdef TIM6
    case JSH_TIMER6: // Not used for outputs
      return TIM6;
#endif
#ifdef TIM7
    case JSH_TIMER7:
      return TIM7;
#endif
#ifdef TIM8
    case JSH_TIMER8:
      return TIM8;
#endif
#ifdef TIM9
    case JSH_TIMER9:
      return TIM9;
#endif
#ifdef TIM10
    case JSH_TIMER10:
      return TIM10;
#endif
#ifdef TIM11
    case JSH_TIMER11:
      return TIM11;
#endif
#ifdef TIM12
    case JSH_TIMER12:
      return TIM12;
#endif
#ifdef TIM13
    case JSH_TIMER13:
      return TIM13;
#endif
#ifdef TIM14
    case JSH_TIMER14:
      return TIM14;
#endif
#ifdef TIM15
    case JSH_TIMER15:
      return TIM15;
#endif
#ifdef TIM16
    case JSH_TIMER16:
      return TIM16;
#endif
#ifdef TIM17
    case JSH_TIMER17:
      return TIM17;
#endif

  }
  return 0;
}

static ALWAYS_INLINE uint8_t stmPinSource(JsvPinInfoPin ipin) {
  JsvPinInfoPin pin = pinInfo[ipin].pin;
  return (uint8_t)(pin-JSH_PIN0);
}

void jshInterruptOff() {
  __disable_irq();
}

void jshInterruptOn() {
  __enable_irq();
}

void jshDelayMicroseconds(int microsec) {
  delayMicroseconds(microsec);
}

void jshPinSetState(Pin pin, JshPinState state) {
  uint16_t _pin = jspin_to_hal(pin);
  PinMode _mode = jspinmode_to_hal(state);
  pinMode(_pin, _mode);
}

JsVarFloat jshPinAnalog(Pin pin) {
  uint16_t _pin = jspin_to_hal(pin);
  JsVarFloat value = (JsVarFloat)analogRead(_pin)/4095;
  return value;
}

int jshPinAnalogFast(Pin pin) {
  return jshPinAnalog(pin);
}

JshPinFunction jshPinAnalogOutput(Pin pin, JsVarFloat value, JsVarFloat freq, JshAnalogOutputFlags flags) { // if freq<=0, the default is used
  uint16_t _pin = jspin_to_hal(pin);
  if(_pin==D10 || _pin==D11) {  //DAC pins
	  wiring_analogWrite(_pin, value*4095);
  }
  else {  // PWM pins
    pinMode(_pin, OUTPUT);
    wiring_analogWrite(_pin, value*255);
  }
  return JSH_NOTHING;
}

void jshSetOutputValue(JshPinFunction func, int value) {
  if (JSH_PINFUNCTION_IS_DAC(func)) {
    uint16_t dacVal = (uint16_t)value;
    switch (func & JSH_MASK_INFO) {
      case JSH_DAC_CH1:  wiring_analogWrite(D10, value); break;
      case JSH_DAC_CH2:  wiring_analogWrite(D11, value); break;
    }
  }
  else if (JSH_PINFUNCTION_IS_TIMER(func)) {
    TIM_TypeDef* TIMx = getTimerFromPinFunction(func);
    if (TIMx) {
      unsigned int period = (unsigned int)TIMx->ARR; // No getter available
      uint16_t timerVal =  (uint16_t)(((unsigned int)value * period) >> 16);
      switch (func & JSH_MASK_TIMER_CH) {
      case JSH_TIMER_CH1:  TIM_SetCompare1(TIMx, timerVal); break;
      case JSH_TIMER_CH2:  TIM_SetCompare2(TIMx, timerVal); break;
      case JSH_TIMER_CH3:  TIM_SetCompare3(TIMx, timerVal); break;
      case JSH_TIMER_CH4:  TIM_SetCompare4(TIMx, timerVal); break;
      }
    }
  } else {
    assert(0); // can't handle this yet...
  }
}

void jshPinSetValue(Pin pin, bool value) {
  uint16_t _pin = jspin_to_hal(pin);
  pinMode(_pin, OUTPUT);
  digitalWrite(_pin, (uint8_t)value);
}

bool jshPinGetValue(Pin pin) {
  uint16_t _pin = jspin_to_hal(pin);
  pinMode(_pin, INPUT);
  return digitalRead(_pin);
}

bool jshIsPinValid(Pin pin) {
  return (pin<JSH_PIN_COUNT && 0xFF!=jspin_to_hal(pin));
}

static uint64_t DEVICE_INITIALISED_FLAGS = 0L;

bool jshIsDeviceInitialised(IOEventFlags device) {
  uint64_t mask = 1ULL << (int)device;
  return (DEVICE_INITIALISED_FLAGS & mask) != 0L;
}

void jshSetDeviceInitialised(IOEventFlags device, bool isInit) {
  uint64_t mask = 1ULL << (int)device;
  if (isInit) {
    DEVICE_INITIALISED_FLAGS |= mask;
  } else {
    DEVICE_INITIALISED_FLAGS &= ~mask;
  }
}

bool jshIsUSBSERIALConnected() {
  return false;
}

JsSysTime jshGetTimeFromMilliseconds(JsVarFloat ms) {
  return (JsSysTime)ms;
}

JsVarFloat jshGetMillisecondsFromTime(JsSysTime time) {
  return (JsVarFloat)time;
}

JsSysTime jshGetSystemTime() {
  return ((JsSysTime)millis());
}

void jshSetSystemTime(JsSysTime time) {
  // Duo does not support setting system time for now.
  return;
}

// ----------------------------------------------------------------------------

void jshPinPulse(Pin pin, bool value, JsVarFloat time) {
}

bool jshCanWatch(Pin pin) {
  if (jshIsPinValid(pin)) {
    return watchedPins[pinInfo[pin].pin]==PIN_UNDEFINED;
  } else
    return false;
}

IOEventFlags jshPinWatch(Pin pin, bool shouldWatch) {
  uint16_t _pin = jspin_to_hal(pin);
  uint8_t pin_source = stmPinSource(pin);
  if (jshIsPinValid(pin)) {
    if (shouldWatch) {
      if (!jshGetPinStateIsManual(pin)) jshPinSetState(pin, JSHPINSTATE_GPIO_IN);
      Interrupt_attachInterrupt(_pin, get_exti_isr(pin_source), CHANGE);
    }
    else Interrupt_detachInterrupt(_pin);

    watchedPins[pinInfo[pin].pin] = (Pin)(shouldWatch ? pin : PIN_UNDEFINED);
    return shouldWatch ? (EV_EXTI0+pinInfo[pin].pin)  : EV_NONE;
  }
  else jsExceptionHere(JSET_ERROR, "Invalid pin!");

  return EV_NONE;
}

bool jshGetWatchedPinState(IOEventFlags device) {
  int exti = IOEVENTFLAGS_GETTYPE(device) - EV_EXTI0;
  Pin pin = watchedPins[exti];
  if (jshIsPinValid(pin))
    return jshPinGetValue(pin);
  return false;
}

bool jshIsEventForPin(IOEvent *event, Pin pin) {
  return IOEVENTFLAGS_GETTYPE(event->flags) == pinToEVEXTI(pin);
}

void jshUSARTSetup(IOEventFlags device, JshUSARTInfo *inf) {
  // Duo has initialized all serial interfaces under lower layer
  return;
}

/** Kick a device into action (if required). For instance we may need
 * to set up interrupts */
void jshUSARTKick(IOEventFlags device) {
  if(device == EV_SERIAL1 || device == EV_SERIAL2) {
    int c = jshGetCharToTransmit(device);
    if (c >= 0) {
      usartserial1_putc(c);
      usbserial_putc(c);
    }
  }
}

static unsigned int jshGetSPIFreq(IOEventFlags device) {
  RCC_ClocksTypeDef clocks;
  RCC_GetClocksFreq(&clocks);
  bool APB2 = device == EV_SPI1;
  return APB2 ? clocks.PCLK2_Frequency : clocks.PCLK1_Frequency;
}

void jshSPISetup(IOEventFlags device, JshSPIInfo *inf) {
  // try and find the best baud rate
  unsigned int spiFreq = jshGetSPIFreq(device);
  const unsigned int baudRatesDivisors[8] = { 2, 4, 8, 16, 32, 64, 128, 256 };
  const uint8_t baudRatesIds[8] = { SPI_CLOCK_DIV2, SPI_CLOCK_DIV4, SPI_CLOCK_DIV8, SPI_CLOCK_DIV16,
                                    SPI_CLOCK_DIV32, SPI_CLOCK_DIV64, SPI_CLOCK_DIV128, SPI_CLOCK_DIV256 };
  int bestDifference = 0x7FFFFFFF;
  unsigned int i;
  uint8_t rate = SPI_CLOCK_DIV128;

  for(i=0; i<8; i++) {
    unsigned int baudrate = spiFreq / baudRatesDivisors[i];

    int rateDiff = inf->baudRate - (int)baudrate;
    if(rateDiff < 0) rateDiff *= -1;

    // if this is outside what we want, make sure it's considered a bad choice
    if(inf->baudRateSpec == SPIB_MAXIMUM && baudrate > (unsigned int)inf->baudRate) rateDiff += 0x10000000;
    if(inf->baudRateSpec == SPIB_MINIMUM && baudrate < (unsigned int)inf->baudRate) rateDiff += 0x10000000;

    if(rateDiff < bestDifference) {
      bestDifference = rateDiff;
      rate = baudRatesIds[i];
    }
  }

  if(device == EV_SPI1) {
    spi_begin();

    spi_setClockDivider(rate);
    spi_setDataMode(inf->spiMode);
    spi_setBitOrder( inf->spiMSB ? MSBFIRST:LSBFIRST );
	jshSetDeviceInitialised(EV_SPI1, true);
  }
  else if(device == EV_SPI2) {
    spi1_begin();

    spi1_setClockDivider(rate);
    spi1_setDataMode(inf->spiMode);
    spi1_setBitOrder( inf->spiMSB ? MSBFIRST:LSBFIRST );
	jshSetDeviceInitialised(EV_SPI2, true);
  }
}

/** Send data through the given SPI device (if data>=0), and return the result
 * of the previous send (or -1). If data<0, no data is sent and the function
 * waits for data to be returned */
int jshSPISend(IOEventFlags device, int data) {
  if(device == EV_SPI1) return (int)spi_transfer((uint8_t) data);
  else if(device == EV_SPI2) return (int)spi1_transfer((uint8_t) data);
  else return -1;
}

/** Send 16 bit data through the given SPI device. */
void jshSPISend16(IOEventFlags device, int data) {
  jshSPISend(device, data>>8);
  jshSPISend(device, data&255);
}

/** Set whether to send 16 bits or 8 over SPI */
void jshSPISet16(IOEventFlags device, bool is16) {
  // Duo does not support 16 bits data mode for now
  return;
}

/** Set whether to use the receive interrupt or not */
void jshSPISetReceive(IOEventFlags device, bool isReceive) {
  // Duo does not use interrupt to received data.
  return;
}

void jshSPIWait(IOEventFlags device) {
  // Duo does not need to wait when sending data.
  return;
}

void jshI2CSetup(IOEventFlags device, JshI2CInfo *inf) {
}

void jshI2CWrite(IOEventFlags device, unsigned char address, int nBytes, const unsigned char *data, bool sendStop) {
}

void jshI2CRead(IOEventFlags device, unsigned char address, int nBytes, unsigned char *data, bool sendStop) {
}

/// Enter simple sleep mode (can be woken up by interrupts). Returns true on success
bool jshSleep(JsSysTime timeUntilWake) {
 //  __WFI(); // Wait for Interrupt
   return true;
}

void* util_timer = NULL;

void util_timer_IRQHandler(void) {
  jstUtilTimerInterruptHandler();
}

void jshUtilTimerDisable(void) {
  if(util_timer != NULL)
    OSTimer_stop(util_timer);
}

void jshUtilTimerReschedule(JsSysTime period) {
  if(util_timer != NULL) {
    OSTimer_changePeriod(util_timer, period);
    OSTimer_start(util_timer);
  }
}

void jshUtilTimerStart(JsSysTime period) {
  if(util_timer == NULL) {
    util_timer = OSTimer_newTimer(period, util_timer_IRQHandler);
    OSTimer_start(util_timer);
  }
  else
    jshUtilTimerReschedule(period);
}

JsVarFloat jshReadTemperature() { return NAN; };
JsVarFloat jshReadVRef()  { return NAN; };
unsigned int jshGetRandomNumber() { return rand(); }

int jshFlashGetSector(uint32_t addr) {
  if (addr >= FLASH_TOTAL+FLASH_START) return -1;
  if (addr < FLASH_START) return -1;

  addr -= FLASH_START;

  if (addr<16*1024) return FLASH_Sector_0;
  else if (addr<32*1024) return FLASH_Sector_1;
  else if (addr<48*1024) return FLASH_Sector_2;
  else if (addr<64*1024) return FLASH_Sector_3;
  else if (addr<128*1024) return FLASH_Sector_4;
  else if (addr<256*1024) return FLASH_Sector_5;
  else if (addr<384*1024) return FLASH_Sector_6;
  else if (addr<512*1024) return FLASH_Sector_7;
  else if (addr<640*1024) return FLASH_Sector_8;
  else if (addr<768*1024) return FLASH_Sector_9;
  else if (addr<896*1024) return FLASH_Sector_10;
  else if (addr<1024*1024) return FLASH_Sector_11;
  assert(0);
  return -1;
}

uint32_t jshFlashGetSectorAddr(int sector) {
  sector /= FLASH_Sector_1; // make an actual int
  if (sector <= 4) return FLASH_START + 16*1024*(uint32_t)sector;
  return FLASH_START + 128*1024*(uint32_t)(sector-4);
}

bool jshFlashGetPage(uint32_t addr, uint32_t *startAddr, uint32_t *pageSize) {
  int sector = jshFlashGetSector(addr);
  if (sector<0) return false;
  if (startAddr) *startAddr = jshFlashGetSectorAddr(sector);
  if (pageSize) *pageSize = jshFlashGetSectorAddr(sector+FLASH_Sector_1)-jshFlashGetSectorAddr(sector);
  return true;
}

static void addFlashArea(JsVar *jsFreeFlash, uint32_t addr, uint32_t length) {
  JsVar *jsArea = jsvNewObject();
  if (!jsArea) return;
  jsvObjectSetChildAndUnLock(jsArea, "addr", jsvNewFromInteger((JsVarInt)addr));
  jsvObjectSetChildAndUnLock(jsArea, "length", jsvNewFromInteger((JsVarInt)length));
  jsvArrayPushAndUnLock(jsFreeFlash, jsArea);
}

JsVar *jshFlashGetFree() {
  JsVar *jsFreeFlash = jsvNewEmptyArray();
  if (!jsFreeFlash) return 0;
  // Try and find the page after the end of firmware
  extern int LINKER_ETEXT_VAR; // end of flash text (binary) section
  uint32_t firmwareEnd = FLASH_START | (uint32_t)&LINKER_ETEXT_VAR;
  uint32_t pAddr, pSize;
  jshFlashGetPage(firmwareEnd, &pAddr, &pSize);
  firmwareEnd = pAddr+pSize;

  if (firmwareEnd < FLASH_SAVED_CODE_START)
    addFlashArea(jsFreeFlash, firmwareEnd, FLASH_SAVED_CODE_START-firmwareEnd);

  // Otherwise add undocumented memory: internal EEPROM1 flash 0x0800c000
  addFlashArea(jsFreeFlash, FLASH_START|(48*1024), 16*1024);

  return jsFreeFlash;
}

void jshFlashErasePage(uint32_t addr) {
  int sector = jshFlashGetSector(addr);
  assert(sector>=0);
  FLASH_Unlock();
  // Clear All pending flags
  FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR |
                  FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR|FLASH_FLAG_PGSERR);
  // Erase
  FLASH_EraseSector((uint32_t)sector, VoltageRange_3);

  FLASH_Lock();
}

void jshFlashRead(void *buf, uint32_t addr, uint32_t len) {
  memcpy(buf, (void*)addr, len);
}

void jshFlashWrite(void *buf, uint32_t addr, uint32_t len) {
  unsigned int i;

  FLASH_Unlock();
  FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR |
                  FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR|FLASH_FLAG_PGSERR);

  for (i=0; i<len; i+=4) {
    while (FLASH_ProgramWord((uint32_t)(addr+i), ((uint32_t*)buf)[i>>2]) != FLASH_COMPLETE);
  }

  FLASH_Lock();
}

// Fake functions to make link stage walk through
void jshReset(void) {
  return;
}

unsigned int jshSetSystemClock(JsVar *options) {
  return 0;
}

JshPinState jshPinGetState(Pin pin)
{
    JshPinState j;
	return j;
}

void jshEnableWatchDog(JsVarFloat timeout) {
  return;
}

JshPinFunction jshGetCurrentPinFunction(Pin pin)
{
    JshPinFunction j;
	return j;
}
