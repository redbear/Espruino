
#include "application.h"
#include "wiring.h"
#include "MDNS.h"

SYSTEM_MODE(SEMI_AUTOMATIC);

MDNS mdns;

void mdns_init()
{
    bool success = mdns.setHostname("duo");
     
    if (success) {
        success = mdns.setService("tcp", "espruino", 8888, "Espruino for Duo");
        Serial.println("setService");
    }

    if (success) {
        success = mdns.begin();
        Serial.println("mdns.begin");
    }
    
    if (success) {
        Spark.publish("mdns/setup", "success");
        Serial.println("mdns/setup success");
    } else {
        Spark.publish("mdns/setup", "error");
        Serial.println("mdns/setup error");
    }
}

void setup()
{
  js_setup();

  mdns_init();  
}

void loop()
{
  js_loop();
  
  mdns.processQueries();
}


