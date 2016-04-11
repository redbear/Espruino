#ifndef _WIFI_API_H
#define _WIFI_API_H

#include "wlan_hal.h"
#include "inet_hal.h"


WLanConfig* wifi_wifiConfig(void);

void wifi_macAddress(uint8_t *mac);
void wifi_localIP(uint32_t *local_ip);
void wifi_subnetMask(uint32_t *net_mask);
void wifi_gatewayIP(uint32_t *gateway_ip);
void wifi_dnsServerIP(uint32_t *dns_ip);
void wifi_dhcpServerIP(uint32_t *dhcp_ip);
void wifi_BSSID(uint8_t* bssid);
const char *SSID(void);

int8_t wifi_RSSI(void);

uint32_t wifi_ping(uint32_t ip, uint8_t nTries);

void wifi_connect(void);
void wifi_disconnect(void);
bool wifi_isConnecting(void);

bool wifi_isReady(void);
void wifi_on(void);
void wifi_off(void);

void wifi_startListen(void);
void wifi_stopListen(void);
bool wifi_isListening(void);

void wifi_setCredentials(const char *ssid, const char *password, unsigned long security, unsigned long cipher);
bool wifi_hasCredentials(void);
bool wifi_clearCredentials(void);
int wifi_getCredentials(WiFiAccessPoint* results, size_t result_count);

void resolve(const char* name, uint32_t *ip);

void wifi_setStaticIP(uint32_t host_ip, uint32_t netmask_ip, uint32_t gateway_ip, uint32_t dns_ip);
void wifi_useStaticIP(void);
void wifi_useDynamicIP(void);

int wifi_scan(WiFiAccessPoint* results, size_t result_count);


#endif
