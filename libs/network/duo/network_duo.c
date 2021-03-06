
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

#include "jsinteractive.h"
#include "network_duo.h"
#include "socketerrors.h"

#include "tcpclient_api.h"
#include "tcpserver_api.h"
#include "wifi_api.h"


typedef enum {
  SOCKET_STATE_UNUSED,
  SOCKET_STATE_USED,
}socket_state_t;

typedef struct {
  tcp_server *server;
  sock_handle_t socket_id;
  socket_state_t socket_state;
}tcp_server_t;

typedef struct {
  tcp_client *client;
  sock_handle_t socket_id;
  socket_state_t socket_state;
}tcp_client_t;

static tcp_server_t servers[MAX_SERVER_SOCKETS];
static tcp_client_t clients[MAX_CLIENT_SOCKETS];

static uint32_t g_nextSocketId = 0;


static uint32_t getNextGlobalSocketId(void);
static int getClientIndexBySocketId(int socketId);
static int getServerIndexBySocketId(int socketId);
static int allocateNewClient(void);
static int allocateNewServer(void);

/**
 * Get the next new global socket id.
 * \return A new socketId that is assured to be unique.
 */
static uint32_t getNextGlobalSocketId(void) {
  if(g_nextSocketId < (SOCKET_INVALID-1)) return ++g_nextSocketId;
  else return SOCKET_INVALID;
}

static int getClientIndexBySocketId(int socketId) {
  int i;
  
  if(socketId <= 0) return -1;
  
  for(i=0; i<MAX_CLIENT_SOCKETS; i++) {
    if(clients[i].socket_id == (uint32_t)socketId) return i;
  }
  return -1;
}

static int getServerIndexBySocketId(int socketId) {
  int i;
  
  if(socketId <= 0) return -1;
  
  for(i=0; i<MAX_SERVER_SOCKETS; i++) {
    if(servers[i].socket_id == (uint32_t)socketId) return i;
  }
  return -1;
}

static int allocateNewClient(void) {
  int i;
  for(i=0; i<MAX_CLIENT_SOCKETS; i++) {
    if(clients[i].socket_state == SOCKET_STATE_UNUSED) return i;
  }

  return -1;
}

static int allocateNewServer(void) {
  int i;
  for(i=0; i<MAX_SERVER_SOCKETS; i++) {
    if(servers[i].socket_state == SOCKET_STATE_UNUSED) return i;
  }

  return -1;
}

/**
 * Initialize the entire socket array
 */
void netInit_duo(void) {
  uint8_t i;
  for(i=0; i<MAX_SERVER_SOCKETS; i++) {
    servers[i].server = NULL;
    servers[i].socket_state = SOCKET_STATE_UNUSED;
  }
  for(i=0; i<MAX_CLIENT_SOCKETS; i++) {
    clients[i].client = NULL;
    clients[i].socket_state = SOCKET_STATE_UNUSED;
  }
}

/**
 * Drop all sockets
 */
void netDisposeAllSockets_duo(void) {
  int index;

  for(index=0; index<MAX_SERVER_SOCKETS; index++) {
    if(servers[index].server != NULL)
      TCPServer_deleteTCPServer(servers[index].server);
  }

  for(index=0; index<MAX_CLIENT_SOCKETS; index++) {
    if(clients[index].client != NULL) {
      TCPClient_stop(clients[index].client);
      TCPClient_deleteTCPClient(clients[index].client);
    }
  }

  netInit_duo();
}

/**
 * Define the implementation functions for the logical network functions.
 */
void netSetCallbacks_duo(JsNetwork *net) {
    net->idle          = net_duo_idle;
    net->checkError    = net_duo_checkError;
    net->createsocket  = net_duo_createSocket;
    net->closesocket   = net_duo_closeSocket;
    net->accept        = net_duo_accept;
    net->gethostbyname = net_duo_gethostbyname;
    net->recv          = net_duo_recv;
    net->send          = net_duo_send;
    net->isconnected   = net_duo_isconnected;
    // The TCP MSS is 536, we use half that 'cause otherwise we easily run out of JSvars memory
    net->chunkSize     = 536/2;
}


/**
 * Check socket connection state.
 */
bool net_duo_isconnected(JsNetwork *net, int sckt) {
  int index;

  index = getClientIndexBySocketId(sckt);
  if(index >= 0) {
    if(clients[index].socket_state == SOCKET_STATE_USED) {
      if(TCPClient_connected(clients[index].client)) {
        return true;
      }
    }
  }

  index = getServerIndexBySocketId(sckt);
  if(index >= 0) {
    if(servers[index].socket_state == SOCKET_STATE_USED) {
      return true;
    }
  }

  return false;
}

/**
 * Determine if there is a new client connection on the server socket.
 * This function is called to poll to see if the serverSckt has a new
 * accepted connection (socket) and, if it does, return it else return -1 to indicate
 * that there was no new accepted socket.
 */
int net_duo_accept(JsNetwork *net, int serverSckt) {
  int index, i;

  if(g_nextSocketId >= (SOCKET_INVALID-1)) return SOCKET_ERR_MAX_SOCK;

  index = getServerIndexBySocketId(serverSckt);
  if(index >= 0) {
    i = allocateNewClient();
    if(i >= 0) {
      clients[i].client = TCPServer_available(servers[index].server);
      if(clients[i].client != NULL) {
        clients[i].socket_id = getNextGlobalSocketId();
        clients[i].socket_state = SOCKET_STATE_USED;
        jsiConsolePrintf(">INFO: New TCP client socket %d connected to server socket %d.\n", clients[i].socket_id, serverSckt);
        return clients[i].socket_id;
      }
    }
  }

  return -1;
}


/**
 * Receive data from the network device.
 * Returns the number of bytes received which may be 0 and <0 if there was an error.
 */
int net_duo_recv(JsNetwork *net, int sckt, void *buf, size_t len) {
  int index;

  index = getClientIndexBySocketId(sckt);
  if(index >= 0) {
    if(TCPClient_available(clients[index].client)) {
      return TCPClient_read(clients[index].client, (uint8_t *)buf, len);
    }
  }

  return 0;
}


/**
 * Send data to the partner.
 * The return is the number of bytes actually transmitted which may also be
 * 0 to indicate no bytes sent or -1 to indicate an error.
 */
int net_duo_send(JsNetwork *net, int sckt, const void *buf, size_t len) {
  int index;

  index = getClientIndexBySocketId(sckt);
  if(index >= 0) {
    int num = TCPClient_write(clients[index].client, (uint8_t *)buf, len);
    if(num == -16){ // WICED_WOULD_BLOCK
      num = 0;
    }
    return num;
  }

  return 0;
}


/**
 * Perform idle processing.
 */
void net_duo_idle(JsNetwork *net) {
  // Do nothing or, should check socket connection ?
}


/**
 * Check for errors.
 * Returns true if there are NO errors.
 */
bool net_duo_checkError(JsNetwork *net) {
  return true;
}

/**
 * Get an IP address from a name.
 */
void net_duo_gethostbyname(JsNetwork *net, char *hostname, uint32_t *outIp) {
  uint8_t host_ip[4];

  wifi_resolve(hostname, host_ip);
  *outIp = (uint32_t)(host_ip[0] | (host_ip[1] << 8) | (host_ip[2] << 16) | (host_ip[3] << 24));
}

/**
 * Create a new socket.
 * if `ipAddress == 0`, creates a server otherwise creates a client (and automatically connects).
 * Returns >=0 on success.
 */
int net_duo_createSocket(JsNetwork *net, uint32_t ipAddress, unsigned short port) {
  int index;
  uint32_t new_socket_id;
  uint8_t ip[4];
  char destIPString[20];
  int destIPLen = 0;

  if(g_nextSocketId >= (SOCKET_INVALID-1)) return SOCKET_ERR_MAX_SOCK;

  if(new_socket_id < SOCKET_INVALID) {
    if(ipAddress == 0) { // Server
      index = allocateNewServer();
      if(index >= 0) {
        servers[index].server = TCPServer_newTCPServer(port);
        if(servers[index].server == NULL) return SOCKET_ERR_MEM;
        if(!TCPServer_begin(servers[index].server)) { // Start TCP server failed!
          TCPServer_deleteTCPServer(servers[index].server);
          return SOCKET_ERR_TIMEOUT;
        }
        new_socket_id = getNextGlobalSocketId();
        servers[index].socket_id = new_socket_id;
        servers[index].socket_state = SOCKET_STATE_USED;
        jsiConsolePrintf(">INFO: New TCP server socket %d listening on port %d\n", new_socket_id, port);
        return new_socket_id;
      }
    }
    else { // Client
      index = allocateNewClient();
      if(index >= 0) {
        clients[index].client = TCPClient_newTCPClient();
        if(clients[index].client == NULL) return SOCKET_ERR_MEM;
        ip[3] = (uint8_t)((ipAddress&0xFF000000) >> 24);
        ip[2] = (uint8_t)((ipAddress&0xFF0000) >> 16);
        ip[1] = (uint8_t)((ipAddress&0xFF00) >> 8);
        ip[0] = (uint8_t)(ipAddress&0xFF);
        if(!TCPClient_connectByIP(clients[index].client, ip, port)) { // Connect to host failed!
          TCPClient_deleteTCPClient(clients[index].client);
          return SOCKET_ERR_TIMEOUT;
        }
        new_socket_id = getNextGlobalSocketId();
        clients[index].socket_id = new_socket_id;
        clients[index].socket_state = SOCKET_STATE_USED;
        destIPLen = jsvGetString(networkGetAddressAsString(ip, 4, 10, '.'), destIPString, sizeof(destIPString)-1);
        destIPString[destIPLen] = '\0';
        jsiConsolePrintf(">INFO: New TCP client socket %d connected to server %s:%d.\n", \
        		new_socket_id, destIPString, port);
        return new_socket_id;
      }
    }
  }

  return SOCKET_ERR_MAX_SOCK;
}

/**
 * Close a socket.
 * This gets called in two situations: when the user requests the close of a socket and as
 * an acknowledgment after we signal the socket library that a connection has closed by
 * returning <0 to a send or recv call.
 */
void net_duo_closeSocket(JsNetwork *net, int socketId) {
  int index;

  index = getServerIndexBySocketId(socketId);
  if(index >= 0) {
    if(servers[index].server != NULL) {
      TCPServer_deleteTCPServer(servers[index].server); // It will close the socket in under layer before deleted
      servers[index].server = NULL;
      servers[index].socket_state = SOCKET_STATE_UNUSED;
      return;
    }
  }

  index = getClientIndexBySocketId(socketId);
  if(index >= 0) {
    if(clients[index].client != NULL) {
      TCPClient_stop(clients[index].client);
      TCPClient_deleteTCPClient(clients[index].client);
      clients[index].client = NULL;
      clients[index].socket_state = SOCKET_STATE_UNUSED;
      return;
    }
  }
}


