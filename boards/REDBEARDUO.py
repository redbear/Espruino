#!/bin/false
# This file is part of Espruino, a JavaScript interpreter for Microcontrollers
#
# Copyright (C) 2013 Gordon Williams <gw@pur3.co.uk>
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# ----------------------------------------------------------------------------------------
# This file contains information for a specific board - the available pins, and where LEDs,
# Buttons, and other in-built peripherals are. It is used to build documentation as well
# as various source and header files for Espruino.
# ----------------------------------------------------------------------------------------

import pinutils;
# placeholder
info = {
 'name' : "RedBear Duo",
 'link' : [ "http://www.RedBear.cc/Duo" ],
 'variables' : 2000,
 'binary_name' : 'espruino_%v_redbearduo.bin',
 'build' : {
  'defines' : [
   ]
 }
};
chip = {
  'part' : "STM32F205RGT6",
  'family' : "STM32F2",
  'package' : "LQFP64",
  'ram' : 128,
  'flash' : 1024,
  'speed' : 120,
  'usart' : 3,
  'spi' : 2,
  'i2c' : 2,
  'adc' : 6,
  'dac' : 0
};

devices = {
};


def get_pins():
#  return []
  pins = pinutils.generate_pins(0,31)  
  pinutils.findpin(pins, "PD0", True)["functions"]["I2C1_SDA"]=0; # Rev 1
  pinutils.findpin(pins, "PD1", True)["functions"]["I2C1_SCL"]=0; # Rev 1
  pinutils.findpin(pins, "PD2", True)["functions"]["I2C1_SDA"]=0; # Rev 2
  pinutils.findpin(pins, "PD3", True)["functions"]["I2C1_SCL"]=0; # Rev 2
  pinutils.findpin(pins, "PD9", True)["functions"]["SPI1_MISO"]=0;
  pinutils.findpin(pins, "PD10", True)["functions"]["SPI1_MOSI"]=0;
  pinutils.findpin(pins, "PD11", True)["functions"]["SPI1_SCK"]=0;
  pinutils.findpin(pins, "PD14", True)["functions"]["UART1_TX"]=0;
  pinutils.findpin(pins, "PD15", True)["functions"]["UART1_RX"]=0;
  return pins