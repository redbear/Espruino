
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
 * Contains Duo board network specific functions.
 * ----------------------------------------------------------------------------
 */

#include "network_duo.h"
#include "socketerrors.h"



/**
 * Define the implementation functions for the logical network functions.
 */
void netSetCallbacks_duo(
    JsNetwork *net //!< The Network we are going to use.
  ) {
    net->idle          = net_duo_idle;
    net->checkError    = net_duo_checkError;
    net->createsocket  = net_duo_createSocket;
    net->closesocket   = net_duo_closeSocket;
    net->accept        = net_duo_accept;
    net->gethostbyname = net_duo_gethostbyname;
    net->recv          = net_duo_recv;
    net->send          = net_duo_send;
    // The TCP MSS is 536, we use half that 'cause otherwise we easily run out of JSvars memory
    net->chunkSize     = 536/2;
}

/**
 * Determine if there is a new client connection on the server socket.
 * This function is called to poll to see if the serverSckt has a new
 * accepted connection (socket) and, if it does, return it else return -1 to indicate
 * that there was no new accepted socket.
 */
int net_duo_accept(
    JsNetwork *net, //!< The Network we are going to use to create the socket.
    int serverSckt  //!< The socket that we are checking to see if there is a new client connection.
) {
  return -1;
}


/**
 * Receive data from the network device.
 * Returns the number of bytes received which may be 0 and <0 if there was an error.
 */
int net_duo_recv(
    JsNetwork *net, //!< The Network we are going to use to create the socket.
    int sckt,       //!< The socket from which we are to receive data.
    void *buf,      //!< The storage buffer into which we will receive data.
    size_t len      //!< The length of the buffer.
) {
    return 0;
}


/**
 * Send data to the partner.
 * The return is the number of bytes actually transmitted which may also be
 * 0 to indicate no bytes sent or -1 to indicate an error.
 */
int net_duo_send(
    JsNetwork *net,  //!< The Network we are going to use to create the socket.
    int sckt,        //!< The socket over which we will send data.
    const void *buf, //!< The buffer containing the data to be sent.
    size_t len       //!< The length of data in the buffer to send.
) {
    return 0;
}


/**
 * Perform idle processing.
 * There is the possibility that we may wish to perform logic when we are idle.
 */
void net_duo_idle(
    JsNetwork *net //!< The Network we are part of.
  ) {
  // Don't echo here because it is called continuously
}


/**
 * Check for errors.
 * Returns true if there are NO errors.
 */
bool net_duo_checkError(
    JsNetwork *net //!< The Network we are checking.
  ) {
  return true;
}


static char *savedHostname = 0;

/**
 * Get an IP address from a name. See the hack description above. This always returns -1
 */
void net_duo_gethostbyname(
    JsNetwork *net, //!< The Network we are going to use to create the socket.
    char *hostname, //!< The string representing the hostname we wish to lookup.
    uint32_t *outIp //!< The address into which the resolved IP address will be stored.
  ) {

}

/**
 * Create a new socket.
 * if `ipAddress == 0`, creates a server otherwise creates a client (and automatically connects).
 * Returns >=0 on success.
 */
int net_duo_createSocket(
    JsNetwork *net,     //!< The Network we are going to use to create the socket.
    uint32_t ipAddress, //!< The address of the partner of the socket or 0 if we are to be a server.
    unsigned short port //!< The port number that the partner is listening upon.
) {

}

/**
 * Close a socket.
 * This gets called in two situations: when the user requests the close of a socket and as
 * an acknowledgment after we signal the socket library that a connection has closed by
 * returning <0 to a send or recv call.
 */
void net_duo_closeSocket(
    JsNetwork *net, //!< The Network we are going to use to create the socket.
    int socketId    //!< The socket to be closed.
) {

}
