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

#include "wifi_api.h"


/*JSON{
   "type": "library",
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
  wifi_on();
}

/*JSON{
  "type"     : "staticmethod",
  "class"    : "WiFi",
  "name"     : "off",
  "generate" : "jswrap_duo_wifi_off"
}
*/
void jswrap_duo_wifi_off(void) {
  // TODO: drop sockets
  wifi_off();
}

/*JSON{
  "type"     : "staticmethod",
  "class"    : "WiFi",
  "name"     : "disconnect",
  "generate" : "jswrap_duo_wifi_disconnect"
}
*/
void jswrap_duo_wifi_disconnect(void) {
  // TODO: drop sockets
  wifi_disconnect();
}

/*JSON{
  "type"     : "staticmethod",
  "class"    : "WiFi",
  "name"     : "connect",
  "generate" : "jswrap_duo_wifi_connect"
}
*/
void jswrap_duo_wifi_connect(void) {
  if(wifi_hasCredentials())
    wifi_connect();
  else
    jsExceptionHere(JSET_ERROR, "No WiFi credentials available in Duo.\n \
            Use WiFi.setCredential({\"ssid\":\"YOUR_SSID\", \"password\":\"YOUR_PASS_WORD\", \"sec\":\"SECUTITY_TYPE\", \"cipher\":\"CIPHER_TYPE\"})\n \
            to set a valid credential first.");
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

  // Make sure jsSetings an object
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
    if(jsvIsStringEqual(jsCipher, "AES")) security = WLAN_CIPHER_AES;
    else if(jsvIsStringEqual(jsCipher, "TKIP")) security = WLAN_CIPHER_TKIP;
    else if(jsvIsStringEqual(jsCipher, "AES_TKIP")) security = WLAN_CIPHER_AES_TKIP;
    else {
      jsvUnLock(jsCipher);
      jsExceptionHere(JSET_ERROR, "Unknown cipher type.");
      return;
    }
  }
  if(jsCipher != NULL) jsvUnLock(jsCipher);

  wifi_setCredentials(ssidString, passwordString, security, cipher);
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

  if(!wifi_isReady()) {
	  jsExceptionHere(JSET_ERROR, "WiFi isn't ready, please wait a moment...");
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

  JsVar *jsLocalIP = jsvNewObject();
  jsvObjectSetChildAndUnLock(jsLocalIP, "SSID", jsvNewFromString(ssid));
  jsvObjectSetChildAndUnLock(jsLocalIP, "Gateway", networkGetAddressAsString(gateway_ip, 4, 10, '.'));
  jsvObjectSetChildAndUnLock(jsLocalIP, "DNS", networkGetAddressAsString(dns_ip, 4, 10, '.'));
  jsvObjectSetChildAndUnLock(jsLocalIP, "DHCP", networkGetAddressAsString(dhcp_ip, 4, 10, '.'));
  jsvObjectSetChildAndUnLock(jsLocalIP, "Net Mask", networkGetAddressAsString(netmask, 4, 10, '.'));
  jsvObjectSetChildAndUnLock(jsLocalIP, "Duo", networkGetAddressAsString(local_ip, 4, 10, '.'));

  // Schedule callback if a function was provided
  if (jsvIsFunction(jsCallback)) {
    JsVar *params[1];
    params[0] = jsLocalIP;
    jsiQueueEvents(NULL, jsCallback, params, 1);
  }

  return jsLocalIP;
}

/*JSON{
  "type" : "init",
  "generate" : "jswrap_duo_wifi_init"
}*/
void jswrap_duo_wifi_init(void) {
  JsNetwork net;
  networkCreate(&net, JSNETWORKTYPE_DUO);
  networkState = NETWORKSTATE_ONLINE;
}
