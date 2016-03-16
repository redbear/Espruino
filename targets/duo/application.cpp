
#include <stddef.h>
#include "platform_config.h"

#ifdef __cplusplus
extern "C" {
#endif
#include "jshardware.h"

#ifdef __cplusplus
}
#endif
#include "jsinteractive.h"
#include "application.h"

#ifdef __cplusplus
extern "C" {
#endif
//void jsiInit(bool autoLoad);
#ifdef __cplusplus
}
#endif

// error handler for pure virtual calls
//void __cxa_pure_virtual() { while (1); }

void setup()
{
  Serial.begin(115200);
  delay(5000);
  Serial.println("Started!");
}

void loop() {
  jshInit();
  jsvInit();
  jsiInit(true);
  
  while (1) 
    jsiLoop();

  // js*Kill()
}

//}

