/*
 * This file is part of Espruino, a JavaScript interpreter for Microcontrollers
 *
 * Copyright (C) 2015 Gordon Williams <gw@pur3.co.uk>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * ----------------------------------------------------------------------------
 * This file is designed to be parsed during the build process
 *
 * Contains Duo board specific function definitions.
 * ----------------------------------------------------------------------------
 */

#ifndef LIBS_NETWORK_DUO_JSWRAP_DUO_NETWORK_H_
#define LIBS_NETWORK_DUO_JSWRAP_DUO_NETWORK_H_

#include "jsvar.h"

void jswrap_duo_wifi_on(void);
void jswrap_duo_wifi_off(void);
void jswrap_duo_wifi_disconnect(void);
void jswrap_duo_wifi_connect(void);
void jswrap_duo_wifi_setCredential(JsVar *jsCredential);
void jswrap_duo_wifi_clearCredentials(void);
JsVar *jswrap_duo_wifi_details(JsVar *jsCallback);
void jswrap_duo_wifi_init(void);

#endif /* LIBS_NETWORK_DUO_JSWRAP_DUO_NETWORK_H_ */
