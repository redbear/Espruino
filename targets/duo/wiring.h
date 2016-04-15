
#include <stddef.h>
#include "platform_config.h"
#include "jshardware.h"
#include "jsinteractive.h"

#include "platforms.h"
#include "usbserial_api.h"
#include "usartserial_api.h"
#include "tcpserver_api.h"
#include "tcpclient_api.h"
#include "wifi_api.h"
#include "gpio_api.h"
#include "ticks_api.h"
#include "timer_api.h"
#include "interrupt_api.h"
#include "spi_api.h"

#ifdef __cplusplus
extern "C" {
#endif

void js_setup(void);
void js_loop(void);


PinMode jspinmode_to_hal(JshPinState state);
Pin jspin_to_hal(Pin pin);

#ifdef __cplusplus
}
#endif
