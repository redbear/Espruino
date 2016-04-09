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

/**
 * The maximum number of concurrently open sockets we support.
 * We should probably pair this with the Duo concept of the maximum number of sockets
 * that an Duo instance can also support.
 */
#define MAX_SOCKETS (10)



#endif /* LIBS_NETWORK_DUO_NETWORK_DUO_H_ */
