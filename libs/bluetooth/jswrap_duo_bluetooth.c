
 /* DO_NOT_INCLUDE_IN_DOCS - this is a special token for common.py, 
so we don't put this into espruino.com/Reference until this is out
of beta.  */

#include "jswrap_duo_bluetooth.h"
#include "jsinteractive.h"
#include "jsdevices.h"

#include "ble_api.h"

#define DEVICE_NAME        "Duo JS Interpreter"

#define CHAR_TX_MAX_LEN    20
#define CHAR_RX_MAX_LEN    20
#define TXRX_BUF_LEN       20

/******************************************************
 *               Variable Definitions
 ******************************************************/
static uint8_t uart_service_uuid[16] = {0x6E, 0x40, 0x00, 0x01, 0xB5, 0xA3, 0xF3, 0x93, 0xE0, 0xA9, 0xE5, 0x0E, 0x24, 0xDC, 0xCA, 0x9E};
static uint8_t uart_char_tx_uuid[16] = {0x6E, 0x40, 0x00, 0x02, 0xB5, 0xA3, 0xF3, 0x93, 0xE0, 0xA9, 0xE5, 0x0E, 0x24, 0xDC, 0xCA, 0x9E};
static uint8_t uart_char_rx_uuid[16] = {0x6E, 0x40, 0x00, 0x03, 0xB5, 0xA3, 0xF3, 0x93, 0xE0, 0xA9, 0xE5, 0x0E, 0x24, 0xDC, 0xCA, 0x9E};

static uint8_t appearance[2] = {
  0x00, 0x02
};

static uint8_t change[2] = { // Service change
  0x00, 0x00
};

static uint8_t conn_param[8] = {
  0x10, 0x00,  // MIN connection interval: 20ms, step by 1.25ms
  0x3C, 0x00,  // MAX connection interval: 20ms, step by 1.25ms
  0x00, 0x00,  // Slave latency
  0xA0, 0x0F   // Connection timeout: 4s, step by 100ms
};

static uint16_t char_tx_value_handle = 0x0000;
static uint16_t char_rx_value_handle = 0x0000;
static uint16_t connect_handle = 0xFFFF;

static uint8_t char_tx_value[CHAR_TX_MAX_LEN];
static uint8_t char_rx_value[CHAR_RX_MAX_LEN];

static advParams_t adv_params;
static uint8_t adv_data[] = {
  // AD flags
  0x02,
  0x01,
  0x06,
  // Local Name
  0x07,
  0x08,
  'D','u','o',' ','J','S',
  // Service UUID
  0x11,
  0x07,
  0x9E,0xCA,0xDC,0x24,0x0E,0xE5,0xA9,0xE0,0x93,0xF3,0xA3,0xB5,0x01,0x00,0x40,0x6E
};

static char rx_buf[TXRX_BUF_LEN];
static uint8_t rx_buf_len;

/**@snippet [Handling the data received over UART] */
static bool  jswrap_duo_ble_transmit_string(void) {
  if (connect_handle == 0xFFFF) {
	// If no connection, drain the output buffer
	while (jshGetCharToTransmit(EV_BLUETOOTH)>=0);
  }

  rx_buf_len = 0;
  int ch = jshGetCharToTransmit(EV_BLUETOOTH);
  while (ch>=0) {
    rx_buf[rx_buf_len++] = ch;
	if(rx_buf_len >= CHAR_RX_MAX_LEN) break;
	ch = jshGetCharToTransmit(EV_BLUETOOTH);
  }
  if (rx_buf_len>0) {
    ble_sendNotify(char_rx_value_handle, (uint8_t*)rx_buf, rx_buf_len);
  }

  return rx_buf_len>0;
}

void jswrap_duo_ble_startAdvertise(void) {
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
  switch (status) {
    case BLE_STATUS_OK:
      connect_handle = handle;
      jsiSetConsoleDevice( EV_BLUETOOTH ); // Is it necessary?
      break;

    default:
      break;
  }
}

static void deviceDisconnectedCallback(uint16_t handle) {
  connect_handle = 0xFFFF;
  jsiSetConsoleDevice( DEFAULT_CONSOLE_DEVICE );
  jswrap_duo_ble_startAdvertise();
}

/**@snippet [Handling the data received over BLE] */
static int gattWriteCallback(uint16_t value_handle, uint8_t *buffer, uint16_t size) {
  uint32_t i;

  if(char_tx_value_handle == value_handle) {
    memcpy(char_tx_value, buffer, (size<CHAR_TX_MAX_LEN ? size : CHAR_TX_MAX_LEN));
    for (i=0; i<size; i++) {
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
  ble_addCharacteristicUUID16(0x2A00, ATT_PROPERTY_READ|ATT_PROPERTY_WRITE, (uint8_t*)DEVICE_NAME, sizeof(DEVICE_NAME));
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
  ble_setAdvData(sizeof(adv_data), adv_data);
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

