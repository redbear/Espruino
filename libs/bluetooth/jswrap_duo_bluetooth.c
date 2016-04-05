
 /* DO_NOT_INCLUDE_IN_DOCS - this is a special token for common.py, 
so we don't put this into espruino.com/Reference until this is out
of beta.  */

#include "jswrap_duo_bluetooth.h"
#include "jsinteractive.h"
#include "jsdevices.h"

#include "ble_api.h"

#define DEVICE_NAME                "Duo JS Interpreter"

#define CHARACTERISTIC1_MAX_LEN    15
#define CHARACTERISTIC2_MAX_LEN    15

#define TXRX_BUF_LEN               15

/******************************************************
 *               Variable Definitions
 ******************************************************/
static uint8_t service1_uuid[16]       ={0x71,0x3d,0x00,0x00,0x50,0x3e,0x4c,0x75,0xba,0x94,0x31,0x48,0xf1,0x8d,0x94,0x1e};
static uint8_t service1_tx_uuid[16]    ={0x71,0x3d,0x00,0x03,0x50,0x3e,0x4c,0x75,0xba,0x94,0x31,0x48,0xf1,0x8d,0x94,0x1e};
static uint8_t service1_rx_uuid[16]    ={0x71,0x3d,0x00,0x02,0x50,0x3e,0x4c,0x75,0xba,0x94,0x31,0x48,0xf1,0x8d,0x94,0x1e};

static uint8_t  appearance[2]    = {0x00, 0x02};
static uint8_t  change[2]        = {0x00, 0x00};
static uint8_t  conn_param[8]    = {0x28, 0x00, 0x90, 0x01, 0x00, 0x00, 0x90, 0x01};

static uint16_t character1_handle = 0x0000;
static uint16_t character2_handle = 0x0000;

static uint16_t connect_handle = 0xFFFF;

static uint8_t characteristic1_data[CHARACTERISTIC1_MAX_LEN]={0x01};
static uint8_t characteristic2_data[CHARACTERISTIC2_MAX_LEN]={0x00};

static advParams_t adv_params;
static uint8_t adv_data[]={0x02,0x01,0x06,0x13,0x08,'D','u','o',' ','J','s',' ','I','n','t','e','r','p','r','e','t','e','r'};

static char rx_buf[TXRX_BUF_LEN];
static uint8_t rx_buf_len;

/**@snippet [Handling the data received over UART] */
static bool  jswrap_nrf_transmit_string(void) {
  if (connect_handle != 0xFFFF) {
	// If no connection, drain the output buffer
	while (jshGetCharToTransmit(EV_BLUETOOTH)>=0);
  }

  rx_buf_len = 0;
  int ch = jshGetCharToTransmit(EV_BLUETOOTH);
  while (ch>=0) {
	  rx_buf[rx_buf_len++] = ch;
	if(rx_buf_len >= CHARACTERISTIC2_MAX_LEN) break;
	ch = jshGetCharToTransmit(EV_BLUETOOTH);
  }
  if (rx_buf_len>0) {
	  ble_sendNotify(character2_handle, (uint8_t*)rx_buf, CHARACTERISTIC2_MAX_LEN);
  }

  return rx_buf_len>0;
}

void jswrap_nrf_bluetooth_startAdvertise(void) {
  adv_params.adv_int_min = 0x00A0;
  adv_params.adv_int_max = 0x01A0;
  adv_params.adv_type    = 0;
  adv_params.dir_addr_type = 0;
  memset(adv_params.dir_addr,0,6);
  adv_params.channel_map = 0x07;
  adv_params.filter_policy = 0x00;

  ble_setAdvParams(&adv_params);
  ble_startAdvertising();
}

static void deviceConnectedCallback(BLEStatus_t status, uint16_t handle) {
  switch (status){
    case BLE_STATUS_OK:
      jsiConsolePrint("Device connected!");
      connect_handle = handle;
      jsiSetConsoleDevice( EV_BLUETOOTH ); // Is it necessary?
      break;
    default:
      break;
  }
}

static void deviceDisconnectedCallback(uint16_t handle) {
  jsiConsolePrint("Disconnected.");
  connect_handle = 0xFFFF;
  jsiSetConsoleDevice( DEFAULT_CONSOLE_DEVICE );
  jswrap_nrf_bluetooth_startAdvertise();
}

/**@snippet [Handling the data received over BLE] */
static int gattWriteCallback(uint16_t value_handle, uint8_t *buffer, uint16_t size) {
  uint32_t i;
  jsiConsolePrint("Write value handler: ");
  jsiConsolePrintf("%x", value_handle);

  if(character1_handle == value_handle) {
    memcpy(characteristic1_data, buffer, (size<CHARACTERISTIC1_MAX_LEN?size:CHARACTERISTIC1_MAX_LEN));

    for (i = 0; i < size; i++) {
      jshPushIOCharEvent(EV_BLUETOOTH, (char)characteristic1_data[i]);
    }
  }
  return 0;
}

/**@brief Function for the SoftDevice initialization.
 *
 * @details This function initializes the SoftDevice and the BLE event interrupt.
 */
static void ble_stack_init(void) {
  ble_init();

  ble_onConnectedCallback(deviceConnectedCallback);
  ble_onDisconnectedCallback(deviceDisconnectedCallback);
  ble_onDataWriteCallback(gattWriteCallback);
}

/**@brief Function for the GAP initialization.
 *
 * @details This function will set up all the necessary GAP (Generic Access Profile) parameters of 
 *          the device. It also sets the permissions and appearance.
 */
static void gap_params_init(void) {
  ble_addServiceUUID16(0x1800);
  ble_addCharacteristicUUID16(0x2A00, ATT_PROPERTY_READ|ATT_PROPERTY_WRITE, (uint8_t*)DEVICE_NAME, sizeof(DEVICE_NAME));
  ble_addCharacteristicUUID16(0x2A01, ATT_PROPERTY_READ, appearance, sizeof(appearance));
  ble_addCharacteristicUUID16(0x2A04, ATT_PROPERTY_READ, conn_param, sizeof(conn_param));
  ble_addServiceUUID16(0x1801);
  ble_addCharacteristicUUID16(0x2A05, ATT_PROPERTY_INDICATE, change, sizeof(change));
}

/**@brief Function for initializing services that will be used by the application.
 */
static void services_init(void) {
  ble_addServiceUUID128(service1_uuid);
  character1_handle = ble_addCharacteristicDynamicUUID128(service1_tx_uuid, ATT_PROPERTY_NOTIFY|ATT_PROPERTY_WRITE|ATT_PROPERTY_WRITE_WITHOUT_RESPONSE, characteristic1_data, CHARACTERISTIC1_MAX_LEN);
  character2_handle = ble_addCharacteristicDynamicUUID128(service1_rx_uuid, ATT_PROPERTY_NOTIFY, characteristic2_data, CHARACTERISTIC2_MAX_LEN);
}

/**@brief Function for initializing the Advertising functionality.
 */
static void advertising_init(void) {
  ble_setAdvData(sizeof(adv_data), adv_data);
}


/*JSON{
    "type": "class",
    "class" : "NRF"
}*/
/*JSON{
  "type" : "object",
  "name" : "Bluetooth",
  "instanceof" : "Serial",
  "#ifdef" : "BLUETOOTH"
}
The USB Serial port
 */

void jswrap_nrf_bluetooth_init(void) {
  ble_stack_init();
  gap_params_init();
  services_init();
  advertising_init();

  jswrap_nrf_bluetooth_wake();
}

/*JSON{
    "type" : "staticmethod",
    "class" : "NRF",
    "name" : "sleep",
    "generate" : "jswrap_nrf_bluetooth_sleep"
}*/
void jswrap_nrf_bluetooth_sleep(void) {
  // If connected, disconnect.
  if (connect_handle != 0xFFFF) {
    ble_disconnect(connect_handle);
  }

  // Stop advertising
  ble_stopAdvertising();
}

/*JSON{
    "type" : "staticmethod",
    "class" : "NRF",
    "name" : "wake",
    "generate" : "jswrap_nrf_bluetooth_wake"
}*/
void jswrap_nrf_bluetooth_wake(void) {
  jswrap_nrf_bluetooth_startAdvertise();
}

/*JSON{
    "type" : "staticmethod",
    "class" : "NRF",
    "name" : "getBattery",
    "generate" : "jswrap_nrf_bluetooth_getBattery",
    "return" : ["float", "Battery level in volts" ]
}
Get the battery level in volts
*/
JsVarFloat jswrap_nrf_bluetooth_getBattery(void) {
  return jshReadVRef();
}

/*JSON{
    "type" : "staticmethod",
    "class" : "NRF",
    "name" : "setAdvertising",
    "generate" : "jswrap_nrf_bluetooth_setAdvertising",
    "params" : [
      ["data","JsVar","The data to advertise as an object - see below for more info"]
    ]
}

Data is of the form `{ UUID : data_as_byte_array }`. For example to return battery level at 95%, do:

```
NRF.setAdvertising({
  0x180F : [95]
});
```

Or you could report the current temperature:

```
setInterval(function() {
  NRF.setAdvertising({
    0x1809 : [0|E.getTemperature()]
  });
}, 30000);
```
*/
void jswrap_nrf_bluetooth_setAdvertising(JsVar *data) {

}

/*JSON{
    "type" : "staticmethod",
    "class" : "NRF",
    "name" : "setScan",
    "generate" : "jswrap_nrf_bluetooth_setScan",
    "params" : [
      ["callback","JsVar","The callback to call with information about received, or undefined to stop"]
    ]
}

Start/stop listening for BLE advertising packets within range...

```
// Start scanning
NRF.setScan(function(d) {
  console.log(JSON.stringify(d,null,2));
});
// prints {"rssi":-72, "addr":"##:##:##:##:##:##", "data":new ArrayBuffer([2,1,6,...])}

// Stop Scanning
NRF.setScan(false);
```
*/
void jswrap_nrf_bluetooth_setScan(JsVar *callback) {

}

/*JSON{
    "type" : "staticmethod",
    "class" : "NRF",
    "name" : "setTxPower",
    "generate" : "jswrap_nrf_bluetooth_setTxPower",
    "params" : [
      ["power","int","Transmit power. Accepted values are -40, -30, -20, -16, -12, -8, -4, 0, and 4 dBm. Others will give an error code."]
    ]
}
Set the BLE radio transmit power. The default TX power is 0 dBm.
*/
void jswrap_nrf_bluetooth_setTxPower(JsVarInt pwr) {
  jsiConsolePrint("Info: Duo does not support setting TX power for now!");
}

/*JSON{
  "type" : "idle",
  "generate" : "jswrap_nrf_idle"
}*/
bool jswrap_nrf_idle() {
  return jswrap_nrf_transmit_string()>0; // return true if we sent anything
}

/*JSON{
  "type" : "kill",
  "generate" : "jswrap_nrf_kill"
}*/
void jswrap_nrf_kill() {
  // Do nothing when execute reset()
}

