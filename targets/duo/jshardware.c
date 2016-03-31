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

#include "wiring.h"

//Timer systemTime;
unsigned int systemTimeHigh;
bool systemTimeWasHigh;

static tcp_server *server = NULL;
static tcp_client *client = NULL;

// ----------------------------------------------------------------------------
// for non-blocking IO
void jshInit() {
  jshInitDevices();

  // turn on WiFi and connect to the AP stored
  //wifi_on();
  //wifi_connect();
  
  // start a tcp server with port number
  server = TCPServer_newTCPServer(TCPPORT);
  TCPServer_begin(server);
  
  // start serial console for user interaction
  // for both USB and serial 1 of the Duo
  usartserial1_begin(9600);
  usbserial_begin(9600);  
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

  /*static bool foo = false;
  foo = !foo;
  jshPinSetValue(LED1_PININDEX, foo);*/

  if (client)
  {
    while (TCPClient_available(client))
    {
        uint8_t c;
        TCPClient_read(client, &c, 1);
        jshPushIOCharEvent(EV_SERIAL1, c);        
    }
  }
  else
  {
      client = TCPServer_available(server);

      if (client)
        jsiConsolePrint(
#ifndef LINUX
          // set up terminal to avoid word wrap
          "\e[?7l"
#endif
          // rectangles @ http://www.network-science.de/ascii/
          "\n"
          " _____                 _ \n"
          "|   __|___ ___ ___ _ _|_|___ ___ \n"
          "|   __|_ -| . |  _| | | |   | . |\n"
          "|_____|___|  _|_| |___|_|_|_|___|\n"
          "          |_| http://espruino.com\n"
          " "JS_VERSION" Copyright 2016 G.Williams\n"
        // Point out about donations - but don't bug people
        // who bought boards that helped Espruino
#if !defined(PICO) && !defined(ESPRUINOBOARD)
          "\n"
          "Espruino is Open Source. Our work is supported\n"
          "only by sales of official boards and donations:\n"
          "http://espruino.com/Donate\n"
#endif
        );
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

void jshInterruptOff() {
}

void jshInterruptOn() {
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

bool jshIsDeviceInitialised(IOEventFlags device) { return true; }

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
}

// ----------------------------------------------------------------------------

void jshPinPulse(Pin pin, bool value, JsVarFloat time) {
}

IOEventFlags jshPinWatch(Pin pin, bool shouldWatch) {
  return EV_NONE;
}

bool jshGetWatchedPinState(IOEventFlags device) {
  return false;
}

bool jshIsEventForPin(IOEvent *event, Pin pin) {
  return false;
}

void jshUSARTSetup(IOEventFlags device, JshUSARTInfo *inf) {
}

/** Kick a device into action (if required). For instance we may need
 * to set up interrupts */
void jshUSARTKick(IOEventFlags device) {
  int id = 0; // TODO: device
  int c = jshGetCharToTransmit(device);

  if (c >= 0) {
    usartserial1_putc(c);
    usbserial_putc(c);

    // if a tcp client connected, also sending out
    if (client)
        TCPClient_write(client, &c, 1);
  }
}

void jshSPISetup(IOEventFlags device, JshSPIInfo *inf) {
}

/** Send data through the given SPI device (if data>=0), and return the result
 * of the previous send (or -1). If data<0, no data is sent and the function
 * waits for data to be returned */
int jshSPISend(IOEventFlags device, int data) {
	return 0;
}

/** Send 16 bit data through the given SPI device. */
void jshSPISend16(IOEventFlags device, int data) {
  jshSPISend(device, data>>8);
  jshSPISend(device, data&255);
}

/** Set whether to send 16 bits or 8 over SPI */
void jshSPISet16(IOEventFlags device, bool is16) {
}

/** Set whether to use the receive interrupt or not */
void jshSPISetReceive(IOEventFlags device, bool isReceive) {
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

void jshUtilTimerDisable() {
}

void jshUtilTimerReschedule(JsSysTime period) {
}

void jshUtilTimerStart(JsSysTime period) {
}

JsVarFloat jshReadTemperature() { return NAN; };
JsVarFloat jshReadVRef()  { return NAN; };
unsigned int jshGetRandomNumber() { return rand(); }

bool jshFlashGetPage(uint32_t addr, uint32_t *startAddr, uint32_t *pageSize) {
  return false;
}

void jshFlashErasePage(uint32_t addr) {
}

void jshFlashRead(void *buf, uint32_t addr, uint32_t len) {
}

void jshFlashWrite(void *buf, uint32_t addr, uint32_t len) {
}

// Fake functions to make link stage walk through
void jshReset(void) {
  return;
}

unsigned int jshSetSystemClock(JsVar *options) {
  return 0;
}

JsVar *jshFlashGetFree()
{
	return 0;
}

JshPinState jshPinGetState(Pin pin)
{
    JshPinState j;
	return j;
}

bool jshCanWatch(Pin pin)
{
	return false;
}

void jshSPIWait(
    IOEventFlags device //!< Unknown
) {
  return;
}

void jshEnableWatchDog(JsVarFloat timeout) {
  return;
}

JshPinFunction jshGetCurrentPinFunction(Pin pin)
{
    JshPinFunction j;
	return j;
}
