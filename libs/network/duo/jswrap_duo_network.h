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

#define MAX_AP_SCAN_RESULT_COUNT    10

void jswrap_duo_wifi_on(void);
void jswrap_duo_wifi_off(void);
void jswrap_duo_wifi_disconnect(void);
void jswrap_duo_wifi_connect(void);
JsVar *jswrap_duo_wifi_isReady(JsVar *jsCallback);
void jswrap_duo_wifi_setCredential(JsVar *jsCredential);
JsVar *jswrap_duo_wifi_getCredentials(JsVar *jsCallback);
void jswrap_duo_wifi_clearCredentials(void);
JsVar *jswrap_duo_wifi_details(JsVar *jsCallback);
JsVar *jswrap_duo_wifi_resolve(JsVar *jsHostName);
JsVar *jswrap_duo_wifi_ping(JsVar *jsDestIP, JsVar *jsnTries);
JsVar *jswrap_duo_wifi_scan(JsVar *jsCallback);
void jswrap_duo_wifi_init(void);
bool jswrap_duo_wifi_idle(void);

#endif /* LIBS_NETWORK_DUO_JSWRAP_DUO_NETWORK_H_ */
