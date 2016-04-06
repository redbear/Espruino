#include "jspin.h"

// public static methods.
void jswrap_duo_ble_init(void);

void jswrap_duo_ble_sleep(void); // maybe these should return err_code?
void jswrap_duo_ble_wake(void);

bool jswrap_duo_ble_idle(void);
void jswrap_duo_ble_kill(void);
