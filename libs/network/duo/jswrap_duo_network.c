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
 * Contains Duo board specific functions.
 * ----------------------------------------------------------------------------
 */

#include "jswrap_duo_network.h"
#include "jswrap_modules.h"
#include "jsinteractive.h"
#include "network.h"
#include "network_duo.h"
#include "jswrap_net.h"
#include "jswrap_telnet.h"

#include "wifi_api.h"


typedef enum {
  WIFI_STATE_OFF,
  WIFI_STATE_ON,
  WIFI_STATE_CONNECTING,
  WIFI_STATE_CONNECTED,
}WiFi_State_t;

static WiFi_State_t wifi_state = WIFI_STATE_OFF;
static bool auto_connect = true;
static bool connect_failed = false; // Temporary variable to fix the bug that it auto-connects to AP when WiFi.on() is called.


/*JSON{
   "type": "class",
   "class": "WiFi"
}
The wifi library is a generic cross-platform library to control the Wifi interface.  It supports functionality such as connecting to wifi networks, getting network information, starting and access point, etc.
*/

/*JSON{
  "type"     : "staticmethod",
  "class"    : "WiFi",
  "name"     : "on",
  "generate" : "jswrap_duo_wifi_on"
}
*/
void jswrap_duo_wifi_on(void) {
  if(!connect_failed) wifi_on(); // Do not turn on WiFi if connect failed before, since it will cause to connect to AP automatically.

  if(wifi_state < WIFI_STATE_ON) wifi_state = WIFI_STATE_ON;
}

/*JSON{
  "type"     : "staticmethod",
  "class"    : "WiFi",
  "name"     : "off",
  "generate" : "jswrap_duo_wifi_off"
}
*/
void jswrap_duo_wifi_off(void) {
#ifdef USE_TELNET
  if(jsiGetConsoleDevice() != EV_TELNET)
#endif
  {
    netDisposeAllSockets_duo();
    wifi_off(); // It will can wifi.disconnect() first
    wifi_state = WIFI_STATE_OFF;
    auto_connect = false;
  }
}

/*JSON{
  "type"     : "staticmethod",
  "class"    : "WiFi",
  "name"     : "disconnect",
  "generate" : "jswrap_duo_wifi_disconnect"
}
*/
void jswrap_duo_wifi_disconnect(void) {
#ifdef USE_TELNET
  if(jsiGetConsoleDevice() != EV_TELNET)
#endif
  {
    netDisposeAllSockets_duo();
    wifi_disconnect(); // Lower layer is responsible for closing all sockets
    if(wifi_state > WIFI_STATE_ON) wifi_state = WIFI_STATE_ON;
    auto_connect = false;
  }
}

/*JSON{
  "type"     : "staticmethod",
  "class"    : "WiFi",
  "name"     : "connect",
  "generate" : "jswrap_duo_wifi_connect"
}
*/
void jswrap_duo_wifi_connect(void) {
  if(wifi_hasCredentials()) {
    wifi_connect(); // It will call wifi.on() first
    wifi_state = WIFI_STATE_CONNECTING;
  }
  else
    jsExceptionHere(JSET_ERROR, "No WiFi credentials available in Duo.\n \
            Use WiFi.setCredential({\"ssid\":\"YOUR_SSID\", \"password\":\"YOUR_PASS_WORD\", \"sec\":\"SECUTITY_TYPE\", \"cipher\":\"CIPHER_TYPE\"})\n \
            to set a valid credential first.");
}

/*JSON{
  "type"     : "staticmethod",
  "class"    : "WiFi",
  "name"     : "isReady",
  "generate" : "jswrap_duo_wifi_isReady",
  "return"   : ["JsVar", "An boolean representing the WiFi connection state to an AP."],
  "params"   : [
    ["callback", "JsVar", "An optional function to be called back with the WiFi connection state, i.e. the same object as returned directly. The callback function is more portable than the direct return value."]
  ]
}
*/
JsVar *jswrap_duo_wifi_isReady(JsVar *jsCallback) {
  // Check callback
  if (jsCallback != NULL && !jsvIsNull(jsCallback) && !jsvIsFunction(jsCallback)) {
    jsExceptionHere(JSET_ERROR, "jsCallback is not an function");
    return NULL;
  }

  bool state = wifi_isReady();

  JsVar *jsConnState = jsvNewFromBool(state);

  // Schedule callback if a function was provided
  if (jsvIsFunction(jsCallback)) {
    JsVar *params[1];
    params[0] = jsConnState;
    jsiQueueEvents(NULL, jsCallback, params, 1);
  }

  return jsConnState;
}

/*JSON{
  "type"     : "staticmethod",
  "class"    : "WiFi",
  "name"     : "setCredential",
  "generate" : "jswrap_duo_wifi_setCredential",
  "params"   : [
    ["jsCredential", "JsVar", "A object contains the WiFi credential"]
  ]
}
*/
void jswrap_duo_wifi_setCredential(JsVar *jsCredential) {
  char ssidString[20+1];
  int ssidLen = 0;
  char passwordString[64+1];
  int passwordLen = 0;
  WLanSecurityType security = WLAN_SEC_NOT_SET;
  WLanSecurityCipher cipher = WLAN_CIPHER_NOT_SET;

  if(wifi_state == WIFI_STATE_OFF)
    jsExceptionHere(JSET_ERROR, "WiFi is turned off. Use WiFi.on() to enable WiFi first.");

  // Make sure jsCredential an object
  if(!jsvIsObject(jsCredential)) {
    jsExceptionHere(JSET_ERROR, "jsCredential is not an object");
	return;
  }

  // ssid
  JsVar *jsSsid = jsvObjectGetChild(jsCredential, "ssid", 0);
  if(jsvIsString(jsSsid)) {
    ssidLen = jsvGetString(jsSsid, ssidString, sizeof(ssidString)-1);
    ssidString[ssidLen] = '\0';
    if(ssidLen == 0) {
      jsvUnLock(jsSsid);
      jsExceptionHere(JSET_ERROR, "The length of SSID is equal to 0");
      return;
    }
  }
  if(jsSsid != NULL) jsvUnLock(jsSsid);

  // password
  JsVar *jsPassword = jsvObjectGetChild(jsCredential, "password", 0);
  if(jsvIsString(jsPassword)) {
    passwordLen = jsvGetString(jsPassword, passwordString, sizeof(passwordString)-1);
    passwordString[passwordLen] = '\0';
  }
  if(jsPassword != NULL) jsvUnLock(jsPassword);

  // security
  JsVar *jsSec = jsvObjectGetChild(jsCredential, "sec", 0);
  if(jsvIsString(jsSec)) {
    if(jsvIsStringEqual(jsSec, "UNSEC")) security = WLAN_SEC_UNSEC;
    else if(jsvIsStringEqual(jsSec, "WEP")) security = WLAN_SEC_WEP;
    else if(jsvIsStringEqual(jsSec, "WPA")) security = WLAN_SEC_WPA;
    else if(jsvIsStringEqual(jsSec, "WPA2")) security = WLAN_SEC_WPA2;
    else {
      jsvUnLock(jsSec);
      jsExceptionHere(JSET_ERROR, "Unknown security type.");
      return;
    }
  }
  if(jsSec != NULL) jsvUnLock(jsSec);

  // cipher
  JsVar *jsCipher = jsvObjectGetChild(jsCredential, "cipher", 0);
  if(jsvIsString(jsCipher)) {
    if(jsvIsStringEqual(jsCipher, "AES")) cipher = WLAN_CIPHER_AES;
    else if(jsvIsStringEqual(jsCipher, "TKIP")) cipher = WLAN_CIPHER_TKIP;
    else if(jsvIsStringEqual(jsCipher, "AES_TKIP")) cipher = WLAN_CIPHER_AES_TKIP;
    else {
      jsvUnLock(jsCipher);
      jsExceptionHere(JSET_ERROR, "Unknown cipher type.");
      return;
    }
  }
  if(jsCipher != NULL) jsvUnLock(jsCipher);

  if(connect_failed) wifi_on(); // To set credential, WiFi must be on. But if connect failed before, then WiFi.on() doesn't turn WiFi on actually, so we turn it on here.
  wifi_setCredentials(ssidString, passwordString, security, cipher);
  if(connect_failed) wifi_off(); // Turn WiFi off to avoid connecting to AP automatically if connect failed before.
}

/*JSON{
  "type"     : "staticmethod",
  "class"    : "WiFi",
  "name"     : "getCredentials",
  "generate" : "jswrap_duo_wifi_getCredentials",
  "return"   : ["JsVar", "An object representing the stored WiFi credentials."],
  "params"   : [
    ["callback", "JsVar", "An optional function to be called back with the stored WiFi credentials, i.e. the same object as returned directly. The callback function is more portable than the direct return value."]
  ]
}
*/
JsVar *jswrap_duo_wifi_getCredentials(JsVar *jsCallback) {
  WiFiAccessPoint storedCredentials[5];
  uint8_t i;

  // Check callback
  if (jsCallback != NULL && !jsvIsNull(jsCallback) && !jsvIsFunction(jsCallback)) {
    jsExceptionHere(JSET_ERROR, "jsCallback is not an function");
    return NULL;
  }

  if(!wifi_hasCredentials()) {
    jsExceptionHere(JSET_ERROR, "No WiFi credentials stored in Duo.");
    return NULL;
  }

  int count = wifi_getCredentials(storedCredentials, 5);

  // Create the Empty JS array that will be passed as a parameter to the callback.
  JsVar *jsCredentialsArray = jsvNewArray(NULL, 0);

  for(i=0; i<count; i++) {
    // Add a new object to the JS array that will be passed as a parameter to the callback.
    // Create, populate and add a child ...
    JsVar *jsCredential = jsvNewObject();

    uint8_t ssidLen = storedCredentials[i].ssidLength;
    storedCredentials[i].ssid[ssidLen] = '\0';
    jsvObjectSetChildAndUnLock(jsCredential, "SSID", jsvNewFromString(storedCredentials[i].ssid));

    if(storedCredentials[i].security == WLAN_SEC_UNSEC)
      jsvObjectSetChildAndUnLock(jsCredential, "Security", jsvNewFromString("OPEN"));
    else if(storedCredentials[i].security == WLAN_SEC_WEP)
      jsvObjectSetChildAndUnLock(jsCredential, "Security", jsvNewFromString("WEP"));
    else if(storedCredentials[i].security == WLAN_SEC_WPA)
      jsvObjectSetChildAndUnLock(jsCredential, "Security", jsvNewFromString("WPA"));
    else
      jsvObjectSetChildAndUnLock(jsCredential, "Security", jsvNewFromString("WPA2"));

    if(storedCredentials[i].cipher == WLAN_CIPHER_AES)
      jsvObjectSetChildAndUnLock(jsCredential, "Cipher", jsvNewFromString("AES"));
    else if(storedCredentials[i].cipher == WLAN_CIPHER_TKIP)
      jsvObjectSetChildAndUnLock(jsCredential, "Cipher", jsvNewFromString("TKIP"));
    else
      jsvObjectSetChildAndUnLock(jsCredential, "Cipher", jsvNewFromString("AES_TKIP"));

    // Add the new record to the array
    jsvArrayPush(jsCredentialsArray, jsCredential);
    jsvUnLock(jsCredential);
  }

  // Schedule callback if a function was provided
  if (jsvIsFunction(jsCallback)) {
    JsVar *params[1];
    params[0] = jsCredentialsArray;
    jsiQueueEvents(NULL, jsCallback, params, 1);
  }

  return jsCredentialsArray;
}

/*JSON{
  "type"     : "staticmethod",
  "class"    : "WiFi",
  "name"     : "clearCredentials",
  "generate" : "jswrap_duo_wifi_clearCredentials"
}
*/
void jswrap_duo_wifi_clearCredentials(void) {
  wifi_clearCredentials();
}

/*JSON{
  "type"     : "staticmethod",
  "class"    : "WiFi",
  "name"     : "details",
  "generate" : "jswrap_duo_wifi_details",
  "return"   : ["JsVar", "An object representing the IP informations."],
  "params"   : [
    ["callback", "JsVar", "An optional function to be called back with the IP informations, i.e. the same object as returned directly. The callback function is more portable than the direct return value."]
  ]
}
*/
JsVar *jswrap_duo_wifi_details(JsVar *jsCallback) {
  uint8_t local_ip[4], netmask[4], gateway_ip[4], dns_ip[4], dhcp_ip[4];
  const char *ssid;

  if(wifi_state < WIFI_STATE_CONNECTED) {
    jsExceptionHere(JSET_ERROR, "WiFi hasn't connected to AP yet.");
    return NULL;
  }

  // Check callback
  if (jsCallback != NULL && !jsvIsNull(jsCallback) && !jsvIsFunction(jsCallback)) {
    jsExceptionHere(JSET_ERROR, "jsCallback is not an function");
    return NULL;
  }

  ssid = wifi_SSID();
  wifi_localIP(local_ip);
  wifi_subnetMask(netmask);
  wifi_gatewayIP(gateway_ip);
  wifi_dnsServerIP(dns_ip);
  wifi_dhcpServerIP(dhcp_ip);

  JsVar *jsDetails = jsvNewObject();
  jsvObjectSetChildAndUnLock(jsDetails, "SSID", jsvNewFromString(ssid));
  jsvObjectSetChildAndUnLock(jsDetails, "Gateway", networkGetAddressAsString(gateway_ip, 4, 10, '.'));
  jsvObjectSetChildAndUnLock(jsDetails, "DNS", networkGetAddressAsString(dns_ip, 4, 10, '.'));
  jsvObjectSetChildAndUnLock(jsDetails, "DHCP", networkGetAddressAsString(dhcp_ip, 4, 10, '.'));
  jsvObjectSetChildAndUnLock(jsDetails, "Net Mask", networkGetAddressAsString(netmask, 4, 10, '.'));
  jsvObjectSetChildAndUnLock(jsDetails, "Duo", networkGetAddressAsString(local_ip, 4, 10, '.'));

  // Schedule callback if a function was provided
  if (jsvIsFunction(jsCallback)) {
    JsVar *params[1];
    params[0] = jsDetails;
    jsiQueueEvents(NULL, jsCallback, params, 1);
  }

  return jsDetails;
}

/*JSON{
  "type"     : "staticmethod",
  "class"    : "WiFi",
  "name"     : "resolve",
  "generate" : "jswrap_duo_wifi_resolve",
  "return"   : ["JsVar", "An string representing the IP address."],
  "params"   : [
    ["jsHostName", "JsVar", "A string contains the host name"]
  ]
}
*/
JsVar *jswrap_duo_wifi_resolve(JsVar *jsHostName) {
  char hostNameString[64+1];
  int hostNameLen = 0;
  uint8_t host_ip[4];

  if(wifi_state < WIFI_STATE_CONNECTED) {
    jsExceptionHere(JSET_ERROR, "WiFi hasn't connected to AP yet.");
    return NULL;
  }

  // Make sure jsHostName a string
  if(!jsvIsString(jsHostName)) {
    jsExceptionHere(JSET_ERROR, "jsHostName is not a string");
	return NULL;
  }

  hostNameLen = jsvGetString(jsHostName, hostNameString, sizeof(hostNameString)-1);
  hostNameString[hostNameLen] = '\0';

  wifi_resolve(hostNameString, host_ip);

  JsVar *jsIP = networkGetAddressAsString(host_ip, 4, 10, '.');

  return jsIP;
}

/*JSON{
  "type"     : "staticmethod",
  "class"    : "WiFi",
  "name"     : "ping",
  "generate" : "jswrap_duo_wifi_ping",
  "return"   : ["JsVar", "An integer representing the successful ping times."],
  "params"   : [
    ["jsDestIP", "JsVar", "A string contains the destination IP address"],
    ["jsnRetry", "JsVar", "A integer contains the retry times"]
  ]
}
*/
JsVar *jswrap_duo_wifi_ping(JsVar *jsDestIP, JsVar *jsnTries) {
  char destIPString[20];
  int destIPLen = 0;
  uint32_t destIP;
  uint8_t ip[4];
  uint32_t nTries;

  if(wifi_state < WIFI_STATE_CONNECTED) {
    jsExceptionHere(JSET_ERROR, "WiFi hasn't connected to AP yet.");
    return NULL;
  }

  // Make sure jsDestIP a string
  if(!jsvIsString(jsDestIP)) {
    jsExceptionHere(JSET_ERROR, "jsDestIP is not a string");
	return NULL;
  }

  // Make sure jsnTries an integer
  if(!jsvIsInt(jsnTries)) {
    jsExceptionHere(JSET_ERROR, "jsnTries is not an integer");
	return NULL;
  }

  destIPLen = jsvGetString(jsDestIP, destIPString, sizeof(destIPString)-1);
  destIPString[destIPLen] = '\0';
  nTries = jsvGetInteger(jsnTries);
  if(nTries == 0) nTries = 3;

  destIP = networkParseIPAddress(destIPString);
  ip[3] = (uint8_t)((destIP&0xFF000000) >> 24);
  ip[2] = (uint8_t)((destIP&0xFF0000) >> 16);
  ip[1] = (uint8_t)((destIP&0xFF00) >> 8);
  ip[0] = (uint8_t)(destIP&0xFF);

  uint32_t count = wifi_ping(ip, nTries);
  JsVar *jsCount = jsvNewFromInteger(count);

  return jsCount;
}

/*JSON{
  "type"     : "staticmethod",
  "class"    : "WiFi",
  "name"     : "scan",
  "generate" : "jswrap_duo_wifi_scan",
  "return"   : ["JsVar", "An object representing the scanned Access Points."],
  "params"   : [
    ["callback", "JsVar", "An optional function to be called back with the scanned Access Points, i.e. the same object as returned directly. The callback function is more portable than the direct return value."]
  ]
}
*/
JsVar *jswrap_duo_wifi_scan(JsVar *jsCallback) {
  WiFiAccessPoint scanResult[MAX_AP_SCAN_RESULT_COUNT];
  uint8_t i;

  // Check callback
  if (jsCallback != NULL && !jsvIsNull(jsCallback) && !jsvIsFunction(jsCallback)) {
    jsExceptionHere(JSET_ERROR, "jsCallback is not an function");
    return NULL;
  }

  if(wifi_state == WIFI_STATE_OFF)
    jsExceptionHere(JSET_ERROR, "WiFi is turned off. Use WiFi.on() to enable WiFi first.");

  int count = wifi_scan(scanResult, MAX_AP_SCAN_RESULT_COUNT);

  // Create the Empty JS array that will be passed as a parameter to the callback.
  JsVar *jsApArray = jsvNewArray(NULL, 0);

  for(i=0; i<count; i++) {
    // Add a new object to the JS array that will be passed as a parameter to the callback.
    // Create, populate and add a child ...
    JsVar *jsAp = jsvNewObject();

    uint8_t ssidLen = scanResult[i].ssidLength;
    scanResult[i].ssid[ssidLen] = '\0';
    jsvObjectSetChildAndUnLock(jsAp, "SSID", jsvNewFromString(scanResult[i].ssid));
    jsvObjectSetChildAndUnLock(jsAp, "BSSID", networkGetAddressAsString(scanResult[i].bssid, 6, 16, ':'));

    if(scanResult[i].security == WLAN_SEC_UNSEC)
      jsvObjectSetChildAndUnLock(jsAp, "Security", jsvNewFromString("OPEN"));
    else if(scanResult[i].security == WLAN_SEC_WEP)
      jsvObjectSetChildAndUnLock(jsAp, "Security", jsvNewFromString("WEP"));
    else if(scanResult[i].security == WLAN_SEC_WPA)
      jsvObjectSetChildAndUnLock(jsAp, "Security", jsvNewFromString("WPA"));
    else
      jsvObjectSetChildAndUnLock(jsAp, "Security", jsvNewFromString("WPA2"));

    if(scanResult[i].cipher == WLAN_CIPHER_AES)
      jsvObjectSetChildAndUnLock(jsAp, "Cipher", jsvNewFromString("AES"));
    else if(scanResult[i].cipher == WLAN_CIPHER_TKIP)
      jsvObjectSetChildAndUnLock(jsAp, "Cipher", jsvNewFromString("TKIP"));
    else
      jsvObjectSetChildAndUnLock(jsAp, "Cipher", jsvNewFromString("AES_TKIP"));

    jsvObjectSetChildAndUnLock(jsAp, "RSSI", jsvNewFromInteger(scanResult[i].rssi));

    // Add the new record to the array
    jsvArrayPush(jsApArray, jsAp);
    jsvUnLock(jsAp);
  }

  // Schedule callback if a function was provided
  if (jsvIsFunction(jsCallback)) {
    JsVar *params[1];
    params[0] = jsApArray;
    jsiQueueEvents(NULL, jsCallback, params, 1);
  }

  return jsApArray;
}

/*JSON{
  "type" : "init",
  "generate" : "jswrap_duo_wifi_init"
}*/
void jswrap_duo_wifi_init(void) {
  JsNetwork net;

  netInit_duo();
  networkCreate(&net, JSNETWORKTYPE_DUO);
  networkState = NETWORKSTATE_ONLINE;
}

/*JSON{
  "type" : "idle",
  "generate" : "jswrap_duo_wifi_idle"
}*/
bool jswrap_duo_wifi_idle(void) {
  static uint8_t retry = 1;

  if(wifi_state == WIFI_STATE_OFF || wifi_state == WIFI_STATE_ON) {
    if(auto_connect && wifi_hasCredentials()) {
      if(retry < 4) {
        jsiConsolePrintf(">INFO: %dth Try connecting to AP ...\n", retry);
        wifi_connect();
        wifi_state = WIFI_STATE_CONNECTING;
        retry++;
      }
      else { // Try 3 times failed
        wifi_off();
        wifi_state = WIFI_STATE_OFF;
        connect_failed = true;
        auto_connect = false;
      }
    }
    else if(!wifi_hasCredentials()) auto_connect = false; // Cancel auto-connect since no credentials stored.
  }

  if(wifi_state == WIFI_STATE_CONNECTING) {
    if(wifi_isReady()) {
      wifi_state = WIFI_STATE_CONNECTED;
      connect_failed = false;
      auto_connect = false;

      JsVar *jsTelnetMode = jsvNewObject();
      jsvObjectSetChildAndUnLock(jsTelnetMode, "mode", jsvNewFromString("on"));
      jswrap_telnet_setOptions(jsTelnetMode);
      jsvUnLock(jsTelnetMode);

      return false;
    }
    else {
      wifi_off();
      wifi_state = WIFI_STATE_OFF;
      connect_failed = true;
    }
  }

  if(wifi_state == WIFI_STATE_CONNECTED) {
    return false;
  }

  return true;
}
