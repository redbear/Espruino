#ifndef _WIRING_MDNS_H
#define _WIRING_MDNS_H


bool mDNS_setHostname(const char *hostname);
bool mDNS_setService(const char *protocol, const char *service, uint16_t port, const char *instance);
bool mDNS_addTXTEntry(const char *key, const char *value);
bool mDNS_begin();
bool mDNS_processQueries();


#endif
