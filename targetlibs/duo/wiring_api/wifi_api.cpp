#include "spark_wiring_wifi.h"

#ifdef __cplusplus
extern "C" {
#endif

using namespace spark;

WLanConfig* wifi_wifiConfig(void)
{
    return WiFi.wifi_config();
}

void wifi_macAddress(uint8_t *mac)
{
	WiFi.macAddress(mac);
}

void wifi_localIP(uint32_t *local_ip)
{
    IPAddress IP;
    IP = WiFi.localIP();
    *local_ip = IP;
}

void wifi_subnetMask(uint32_t *net_mask)
{
    IPAddress IP;
    IP = WiFi.subnetMask();
    *net_mask = IP;
}

void wifi_gatewayIP(uint32_t *gateway_ip)
{
    IPAddress IP;
    IP = WiFi.gatewayIP();
    *gateway_ip = IP;
}

void wifi_dnsServerIP(uint32_t *dns_ip)
{
    IPAddress IP;
    IP = WiFi.dnsServerIP();
    *dns_ip = IP;
}

void wifi_dhcpServerIP(uint32_t *dhcp_ip)
{
    IPAddress IP;
    IP = WiFi.dhcpServerIP();
    *dhcp_ip = IP;
}

void wifi_BSSID(uint8_t* bssid)
{
	WiFi.BSSID(bssid);
}

const char *SSID(void)
{
    return WiFi.SSID();
}

int8_t wifi_RSSI(void)
{
    return WiFi.RSSI();
}

uint32_t wifi_ping(uint32_t ip, uint8_t nTries)
{
	IPAddress remoteIP = ip;
    return WiFi.ping(remoteIP, nTries);
}

void wifi_connect(void)
{
	WiFi.connect();
}

void wifi_disconnect(void)
{
	WiFi.disconnect();
}

bool wifi_isConnecting(void)
{
    return WiFi.connecting();
}

bool wifi_isReady(void)
{
    return WiFi.ready();
}

void wifi_on(void)
{
	WiFi.on();
}

void wifi_off(void)
{
	WiFi.off();
}

void wifi_startListen(void)
{
	WiFi.listen();
}

void wifi_stopListen(void)
{
	WiFi.listen(false);
}

bool wifi_isListening(void)
{
    return WiFi.listening();
}

void wifi_setCredentials(const char *ssid, const char *password, unsigned long security, unsigned long cipher)
{
    WiFi.setCredentials(ssid, password, security, cipher);
}

bool wifi_hasCredentials(void)
{
    return WiFi.hasCredentials();
}

bool wifi_clearCredentials(void)
{
    return WiFi.clearCredentials();
}

void resolve(const char* name, uint32_t *ip)
{
    IPAddress IP;
    IP = WiFi.resolve(name);
    *ip = IP;
}

void wifi_setStaticIP(uint32_t host_ip, uint32_t netmask_ip, uint32_t gateway_ip, uint32_t dns_ip)
{
    IPAddress host, netmask, gateway, dns;
    IPAddress &_host=host;
    IPAddress &_netmask=netmask;
    IPAddress &_gateway=gateway;
    IPAddress &_dns=dns;

    host = host_ip;
    netmask = netmask_ip;
    gateway = gateway_ip;
    dns = dns_ip;

    WiFi.setStaticIP(_host, _netmask, _gateway, _dns);
}

void wifi_useStaticIP(void)
{
	WiFi.useStaticIP();
}

void wifi_useDynamicIP(void)
{
	WiFi.useDynamicIP();
}

int wifi_scan(WiFiAccessPoint* results, size_t result_count)
{
    return WiFi.scan(results, result_count);
}

int wifi_getCredentials(WiFiAccessPoint* results, size_t result_count)
{
    return WiFi.getCredentials(results, result_count);
}


#ifdef __cplusplus
}
#endif
