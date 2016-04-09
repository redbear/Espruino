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


//===== This file contains definitions for class: Duo_Wifi


/*JSON{
   "type": "library",
   "class": "Duo_Wifi"
}
The wifi library is a generic cross-platform library to control the Wifi interface.  It supports functionality such as connecting to wifi networks, getting network information, starting and access point, etc.

To get started and connect to your local access point all you need is
```
var wifi = require("Duo_Wifi");
wifi.connect("my-ssid", {password:"my-pwd"}, function(ap){ console.log("connected:", ap); });
```

*/

//===== Functions

//===== Duo_Wifi.disconnect

/*JSON{
  "type"     : "staticmethod",
  "class"    : "Duo_Wifi",
  "name"     : "disconnect",
  "generate" : "jswrap_duo_wifi_disconnect",
  "params"   : [
    ["callback", "JsVar", "An optional function to be called back on disconnection. The callback function receives no argument."]
  ]
}
Disconnect the wifi station from an access point and disable the station mode. It is OK to call `disconnect` to turn off station mode even if no connection exists (for example, connection attempts may be failing). Station mode can be re-enabled by calling `connect` or `scan`.
*/
void jswrap_duo_wifi_disconnect(JsVar *jsCallback) {

}

//===== Duo_Wifi.connect

/*JSON{
  "type"     : "staticmethod",
  "class"    : "Duo_Wifi",
  "name"     : "connect",
  "generate" : "jswrap_duo_wifi_connect",
  "params"   : [
    ["ssid", "JsVar", "The access point network id."],
    ["options", "JsVar", "Connection options (optional)."],
    ["callback", "JsVar", "A function to be called back on completion (optional)."]
  ]
}
Connect to an access point as a station. If there is an existing connection to an AP it is first disconnected if the SSID or password are different from those passed as parameters. Put differently, if the passed SSID and password are identical to the currently connected AP then nothing is changed.
When the connection attempt completes the callback function is invoked with one `err` parameter, which is NULL if there is no error and a string message if there is an error. If DHCP is enabled the callback occurs once an IP addres has been obtained, if a static IP is set the callback occurs once the AP's network has been joined.  The callback is also invoked if a connection already exists and does not need to be changed.

The options properties may contain:

* `password` - Password string to be used to access the network.
* `dnsServers` (array of String) - An array of up to two DNS servers in dotted decimal format string.

Notes:

* the options should include the ability to set a static IP and associated netmask and gateway, this is a future enhancement.
* the only error reported in the callback is "Bad password", all other errors (such as access point not found or DHCP timeout) just cause connection retries. If the reporting of such temporary errors is desired, the caller must use its own timeout and the `getDetails().status` field.
* the `connect` call automatically enabled station mode, it can be disabled again by calling `disconnect`.

*/
void jswrap_duo_wifi_connect(
    JsVar *jsSsid,
    JsVar *jsOptions,
    JsVar *jsCallback
  ) {

}

/*JSON{
  "type" : "init",
  "generate" : "jswrap_duo_wifi_init"
}*/
void jswrap_duo_wifi_init() {
  JsNetwork net;
  networkCreate(&net, JSNETWORKTYPE_DUO);
  networkState = NETWORKSTATE_ONLINE;
}
