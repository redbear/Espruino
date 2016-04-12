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
 * Contains Duo board network specific function definitions.
 * ----------------------------------------------------------------------------
 */

#ifndef LIBS_NETWORK_DUO_NETWORK_DUO_H_
#define LIBS_NETWORK_DUO_NETWORK_DUO_H_

#include "network.h"

#define MAX_SERVER_SOCKETS  3
#define MAX_CLIENT_SOCKETS  10
#define SOCKET_UNUSED  0x0
#define SOCKET_INVALID  0x7FFFFFFF

void netInit_duo(void);
void netSetCallbacks_duo(JsNetwork *net);
int  net_duo_accept(JsNetwork *net, int serverSckt);
int  net_duo_recv(JsNetwork *net, int sckt, void *buf, size_t len);
int  net_duo_send(JsNetwork *net, int sckt, const void *buf, size_t len);
void net_duo_idle(JsNetwork *net);
bool net_duo_checkError(JsNetwork *net);
int  net_duo_createSocket(JsNetwork *net, uint32_t ipAddress, unsigned short port);
void net_duo_closeSocket(JsNetwork *net, int sckt);
void net_duo_gethostbyname(JsNetwork *net, char *hostName, uint32_t *outIp);


#endif /* LIBS_NETWORK_DUO_NETWORK_DUO_H_ */
