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
 'default_console' : "EV_SERIAL1",
 'variables' : 2250,
 'bootloader' : 0,
 'binary_name' : 'espruino_%v_redbearduo.bin',
 'build' : {
  'defines' : [
   ]
 }
};
chip = {
  'part' : "STM32F205RGT6",
  'family' : "STM32F2",
  'package' : "WLCSP64",
  'ram' : 128,
  'flash' : 1024,
  'speed' : 120,
  'usart' : 2,
  'spi' : 2,
  'i2c' : 1,
  'adc' : 8,
  'dac' : 2,
  'saved_code' : {
    'address' : 0x080E0000, # last page start address
    'page_size' : 131072, # size of pages
    'pages' : 1, # number of pages we're using
    'flash_available' : 256 # 256KB internal flash used for user part
  },
};

devices = {
  'LED1' : { 'pin' : 'B11' }, # R
  'LED2' : { 'pin' : 'B1' },  # G
  'LED3' : { 'pin' : 'B0' },  # B
  'LED4' : { 'pin' : 'A13' }, # user led
  'BTN1' : { 'pin' : 'B2' },
};


def get_pins():
  pins = pinutils.scan_pin_file([], 'stm32f20x.csv', 6, 9, 10)
  pins = pinutils.scan_pin_af_file(pins, 'stm32f20x_af.csv', 0, 1)
  return pinutils.only_from_package(pinutils.fill_gaps_in_pin_list(pins), chip["package"])
  
  
