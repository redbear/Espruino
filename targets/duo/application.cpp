
#include "application.h"
#include "wiring.h"

SYSTEM_MODE(MANUAL);

void setup()
{
  Serial.begin(115200);
  delay(5000);
  Serial.println("Started!");

  js_setup();
}

void loop() {

  js_loop();

}


