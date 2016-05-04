#include "MDNS.h"

MDNS mdns;

#ifdef __cplusplus
extern "C" {
#endif


bool mDNS_setHostname(const char *hostname) {
  return mdns.setHostname(hostname);
}

bool mDNS_setService(const char *protocol, const char *service, uint16_t port, const char *instance) {
  return mdns.setService(protocol, service, port, instance);
}

bool mDNS_addTXTEntry(const char *key, const char *value) {
  return mdns.addTXTEntry(key, value);
}

bool mDNS_begin() {
  return mdns.begin();
}

bool mDNS_processQueries() {
  return mdns.processQueries();
}


#ifdef __cplusplus
}
#endif
