
#include "application.h"
#include "wiring.h"
#include "MDNS.h"

SYSTEM_MODE(SEMI_AUTOMATIC);

MDNS mdns;

void mdns_init()
{
    bool success = mdns.setHostname("duo");
     
    if (success) {
        success = mdns.setService("tcp", "espruino", TCPPORT, "Espruino for RedBear Duo");

        success = mdns.begin();
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


