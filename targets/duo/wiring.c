
#include <stddef.h>
#include "platform_config.h"
#include "jshardware.h"
#include "jsinteractive.h"

void js_setup() {
  jshInit();
  jsvInit();
  jsiInit(true);
}

void js_loop() {
  jsiLoop();

  // js*Kill()
}

