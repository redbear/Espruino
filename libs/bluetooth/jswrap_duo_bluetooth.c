
 /* DO_NOT_INCLUDE_IN_DOCS - this is a special token for common.py, 
so we don't put this into espruino.com/Reference until this is out
of beta.  */

#include "jswrap_duo_bluetooth.h"
#include "jsinteractive.h"
#include "jsdevices.h"

#include "ble_api.h"

/*
 * BLE peripheral preferred connection parameters:
 *     - Minimum connection interval = MIN_CONN_INTERVAL * 1.25 ms, where MIN_CONN_INTERVAL ranges from 0x0006 to 0x0C80
 *     - Maximum connection interval = MAX_CONN_INTERVAL * 1.25 ms,  where MAX_CONN_INTERVAL ranges from 0x0006 to 0x0C80
 *     - The SLAVE_LATENCY ranges from 0x0000 to 0x03E8
 *     - Connection supervision timeout = CONN_SUPERVISION_TIMEOUT * 10 ms, where CONN_SUPERVISION_TIMEOUT ranges from 0x000A to 0x0C80
 */
#define MIN_CONN_INTERVAL          0x0028 // 50ms.
#define MAX_CONN_INTERVAL          0x0190 // 500ms.
#define SLAVE_LATENCY              0x0000 // No slave latency.
#define CONN_SUPERVISION_TIMEOUT   0x03E8 // 10s.

// Learn about appearance: http://developer.bluetooth.org/gatt/characteristics/Pages/CharacteristicViewer.aspx?u=org.bluetooth.characteristic.gap.appearance.xml
#define BLE_PERIPHERAL_APPEARANCE  BLE_APPEARANCE_UNKNOWN

#define BLE_DEVICE_NAME        "Duo JS Interpreter"

#define CHAR_TX_MAX_LEN    20
#define CHAR_RX_MAX_LEN    20
#define TXRX_BUF_LEN       20

/******************************************************
 *               Variable Definitions
 ******************************************************/
static uint8_t uart_service_uuid[16] = { 0x6E, 0x40, 0x00, 0x01, 0xB5, 0xA3, 0xF3, 0x93, 0xE0, 0xA9, 0xE5, 0x0E, 0x24, 0xDC, 0xCA, 0x9E };
static uint8_t uart_char_tx_uuid[16] = { 0x6E, 0x40, 0x00, 0x02, 0xB5, 0xA3, 0xF3, 0x93, 0xE0, 0xA9, 0xE5, 0x0E, 0x24, 0xDC, 0xCA, 0x9E };
static uint8_t uart_char_rx_uuid[16] = { 0x6E, 0x40, 0x00, 0x03, 0xB5, 0xA3, 0xF3, 0x93, 0xE0, 0xA9, 0xE5, 0x0E, 0x24, 0xDC, 0xCA, 0x9E };

// GAP and GATT characteristics value
static uint8_t  appearance[2] = {
  LOW_BYTE(BLE_PERIPHERAL_APPEARANCE),
  HIGH_BYTE(BLE_PERIPHERAL_APPEARANCE)
};

static uint8_t  change[4] = {
  0x00, 0x00, 0xFF, 0xFF
};

static uint8_t  conn_param[8] = {
  LOW_BYTE(MIN_CONN_INTERVAL), HIGH_BYTE(MIN_CONN_INTERVAL),
  LOW_BYTE(MAX_CONN_INTERVAL), HIGH_BYTE(MAX_CONN_INTERVAL),
  LOW_BYTE(SLAVE_LATENCY), HIGH_BYTE(SLAVE_LATENCY),
  LOW_BYTE(CONN_SUPERVISION_TIMEOUT), HIGH_BYTE(CONN_SUPERVISION_TIMEOUT)
};

/*
 * BLE peripheral advertising parameters:
 *     - advertising_interval_min: [0x0020, 0x4000], default: 0x0800, unit: 0.625 msec
 *     - advertising_interval_max: [0x0020, 0x4000], default: 0x0800, unit: 0.625 msec
 *     - advertising_type:
 *           BLE_GAP_ADV_TYPE_ADV_IND
 *           BLE_GAP_ADV_TYPE_ADV_DIRECT_IND
 *           BLE_GAP_ADV_TYPE_ADV_SCAN_IND
 *           BLE_GAP_ADV_TYPE_ADV_NONCONN_IND
 *     - own_address_type:
 *           BLE_GAP_ADDR_TYPE_PUBLIC
 *           BLE_GAP_ADDR_TYPE_RANDOM
 *     - advertising_channel_map:
 *           BLE_GAP_ADV_CHANNEL_MAP_37
 *           BLE_GAP_ADV_CHANNEL_MAP_38
 *           BLE_GAP_ADV_CHANNEL_MAP_39
 *           BLE_GAP_ADV_CHANNEL_MAP_ALL
 *     - filter policies:
 *           BLE_GAP_ADV_FP_ANY
 *           BLE_GAP_ADV_FP_FILTER_SCANREQ
 *           BLE_GAP_ADV_FP_FILTER_CONNREQ
 *           BLE_GAP_ADV_FP_FILTER_BOTH
 *
 * Note:  If the advertising_type is set to BLE_GAP_ADV_TYPE_ADV_SCAN_IND or BLE_GAP_ADV_TYPE_ADV_NONCONN_IND,
 *        the advertising_interval_min and advertising_interval_max should not be set to less than 0x00A0.
 */
static advParams_t adv_params = {
  .adv_int_min   = 0x0030,
  .adv_int_max   = 0x0030,
  .adv_type      = BLE_GAP_ADV_TYPE_ADV_IND,
  .dir_addr_type = BLE_GAP_ADDR_TYPE_PUBLIC,
  .dir_addr      = {0,0,0,0,0,0},
  .channel_map   = BLE_GAP_ADV_CHANNEL_MAP_ALL,
  .filter_policy = BLE_GAP_ADV_FP_ANY
};

// BLE peripheral advertising data
static uint8_t adv_data[] = {
  0x02,
  BLE_GAP_AD_TYPE_FLAGS,
  BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE,

  0x11,
  BLE_GAP_AD_TYPE_128BIT_SERVICE_UUID_COMPLETE,
  0x9E,0xCA,0xDC,0x24,0x0E,0xE5,0xA9,0xE0,0x93,0xF3,0xA3,0xB5,0x01,0x00,0x40,0x6E
};

// BLE peripheral scan respond data
static uint8_t scan_response[] = {
  0x07,
  BLE_GAP_AD_TYPE_COMPLETE_LOCAL_NAME,
  'D','u','o',' ','J','S',
};

static uint16_t char_tx_value_handle = 0x0000;
static uint16_t char_rx_value_handle = 0x0000;
static uint16_t connect_handle = BLE_CONN_HANDLE_INVALID ;

static uint8_t char_tx_value[CHAR_TX_MAX_LEN];
static uint8_t char_rx_value[CHAR_RX_MAX_LEN];

static char rx_buf[TXRX_BUF_LEN];
static uint8_t rx_buf_len;


/**@snippet [Handling the data received over UART] */
static bool  jswrap_duo_ble_transmit_string(void) {
  if (connect_handle == BLE_CONN_HANDLE_INVALID ) {
	// If no connection, drain the output buffer
	while (jshGetCharToTransmit(EV_BLUETOOTH) >= 0);
  }

  rx_buf_len = 0;
  int ch = jshGetCharToTransmit(EV_BLUETOOTH);
  while (ch >= 0) {
    rx_buf[rx_buf_len++] = ch;
	if(rx_buf_len >= CHAR_RX_MAX_LEN) break;
	ch = jshGetCharToTransmit(EV_BLUETOOTH);
  }
  if (rx_buf_len > 0) {
    ble_sendNotify(char_rx_value_handle, (uint8_t*)rx_buf, rx_buf_len);
  }

  return rx_buf_len>0;
}

void jswrap_duo_ble_startAdvertise(void) {
  ble_startAdvertising();
}

static void deviceConnectedCallback(BLEStatus_t status, uint16_t handle) {
  switch (status) {
    case BLE_STATUS_OK:
      connect_handle = handle;
      jshSetDeviceInitialised(EV_BLUETOOTH, true);
      if (!jsiIsConsoleDeviceForced()) jsiSetConsoleDevice(EV_BLUETOOTH, false);
      break;

    default:
      break;
  }
}

static void deviceDisconnectedCallback(uint16_t handle) {
  connect_handle = BLE_CONN_HANDLE_INVALID ;
  if (!jsiIsConsoleDeviceForced()) jsiSetConsoleDevice(DEFAULT_CONSOLE_DEVICE, 0);
  jswrap_duo_ble_startAdvertise();
  jshSetDeviceInitialised(EV_BLUETOOTH, false);
}

/**@snippet [Handling the data received over BLE] */
static int gattWriteCallback(uint16_t value_handle, uint8_t *buffer, uint16_t size) {
  uint32_t i;

  if(char_tx_value_handle == value_handle) {
    memcpy(char_tx_value, buffer, (size<CHAR_TX_MAX_LEN ? size : CHAR_TX_MAX_LEN));
    for (i = 0; i < size; i++) {
      jshPushIOCharEvent(EV_BLUETOOTH, (char)char_tx_value[i]);
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
  ble_addCharacteristicUUID16(0x2A00, ATT_PROPERTY_READ|ATT_PROPERTY_WRITE, (uint8_t*)BLE_DEVICE_NAME, sizeof(BLE_DEVICE_NAME));
  ble_addCharacteristicUUID16(0x2A01, ATT_PROPERTY_READ, appearance, sizeof(appearance));
  ble_addCharacteristicUUID16(0x2A04, ATT_PROPERTY_READ, conn_param, sizeof(conn_param));
  ble_addServiceUUID16(0x1801);
  ble_addCharacteristicUUID16(0x2A05, ATT_PROPERTY_INDICATE, change, sizeof(change));
}

/**@brief Function for initializing services that will be used by the application.
 */
static void services_init(void) {
  ble_addServiceUUID128(uart_service_uuid);
  // The order of adding characteristics matters a lot!
  char_rx_value_handle = ble_addCharacteristicDynamicUUID128(uart_char_rx_uuid, ATT_PROPERTY_NOTIFY, char_rx_value, CHAR_RX_MAX_LEN);
  char_tx_value_handle = ble_addCharacteristicDynamicUUID128(uart_char_tx_uuid, ATT_PROPERTY_WRITE_WITHOUT_RESPONSE|ATT_PROPERTY_WRITE, char_tx_value, CHAR_TX_MAX_LEN);
}

/**@brief Function for initializing the Advertising functionality.
 */
static void advertising_init(void) {
  ble_setAdvParams(&adv_params);
  ble_setAdvData(sizeof(adv_data), adv_data);
  ble_setScanRspData(sizeof(scan_response), scan_response);
}


/*JSON{
    "type": "class",
    "class" : "RBLE"
}*/

/*JSON{
  "type" : "init",
  "generate" : "jswrap_duo_ble_init"
}*/
void jswrap_duo_ble_init(void) {
  ble_stack_init();
  gap_params_init();
  services_init();
  advertising_init();

  jswrap_duo_ble_wake();
}

/*JSON{
  "type" : "idle",
  "generate" : "jswrap_duo_ble_idle"
}*/
bool jswrap_duo_ble_idle() {
  return jswrap_duo_ble_transmit_string()>0; // return true if we sent anything
}

/*JSON{
  "type" : "kill",
  "generate" : "jswrap_duo_ble_kill"
}*/
void jswrap_duo_ble_kill() {
  jswrap_duo_ble_sleep();
}

/*JSON{
    "type" : "staticmethod",
    "class" : "RBLE",
    "name" : "sleep",
    "generate" : "jswrap_duo_ble_sleep"
}*/
void jswrap_duo_ble_sleep(void) {
  // If connected, disconnect.
  if (connect_handle != 0xFFFF) {
    ble_disconnect(connect_handle);
  }

  // Stop advertising
  ble_stopAdvertising();
}

/*JSON{
    "type" : "staticmethod",
    "class" : "RBLE",
    "name" : "wake",
    "generate" : "jswrap_duo_ble_wake"
}*/
void jswrap_duo_ble_wake(void) {
  jswrap_duo_ble_startAdvertise();
}

