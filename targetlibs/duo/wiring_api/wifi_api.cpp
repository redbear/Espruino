#include "spark_wiring_wifi.h"

#ifdef __cplusplus
extern "C" {
#endif


WLanConfig* wifi_wifiConfig(void)
{
    return spark::WiFi.wifi_config();
}

void wifi_macAddress(uint8_t *mac)
{
	spark::WiFi.macAddress(mac);
}

void wifi_localIP(uint32_t *local_ip)
{
    IPAddress IP;
    IP = spark::WiFi.localIP();
    *local_ip = IP;
}

void wifi_subnetMask(uint32_t *net_mask)
{
    IPAddress IP;
    IP = spark::WiFi.subnetMask();
    *net_mask = IP;
}

void wifi_gatewayIP(uint32_t *gateway_ip)
{
    IPAddress IP;
    IP = spark::WiFi.gatewayIP();
    *gateway_ip = IP;
}

void wifi_dnsServerIP(uint32_t *dns_ip)
{
    IPAddress IP;
    IP = spark::WiFi.dnsServerIP();
    *dns_ip = IP;
}

void wifi_dhcpServerIP(uint32_t *dhcp_ip)
{
    IPAddress IP;
    IP = spark::WiFi.dhcpServerIP();
    *dhcp_ip = IP;
}

void wifi_BSSID(uint8_t* bssid)
{
	spark::WiFi.BSSID(bssid);
}

const char *SSID(void)
{
    return spark::WiFi.SSID();
}

int8_t wifi_RSSI(void)
{
    return spark::WiFi.RSSI();
}

uint32_t wifi_ping(uint32_t ip, uint8_t nTries)
{
	IPAddress remoteIP = ip;
    return spark::WiFi.ping(remoteIP, nTries);
}

void wifi_connect(void)
{
	spark::WiFi.connect();
}

void wifi_disconnect(void)
{
	spark::WiFi.disconnect();
}

bool wifi_isConnecting(void)
{
    return spark::WiFi.connecting();
}

bool wifi_isReady(void)
{
    return spark::WiFi.ready();
}

void wifi_on(void)
{
	spark::WiFi.on();
}

void wifi_off(void)
{
	spark::WiFi.off();
}

void wifi_startListen(void)
{
	spark::WiFi.listen();
}

void wifi_stopListen(void)
{
	spark::WiFi.listen(false);
}

bool wifi_isListening(void)
{
    return spark::WiFi.listening();
}

void wifi_setCredentials(const char *ssid, const char *password, unsigned long security)
{
	spark::WiFi.setCredentials(ssid, password, security);
}

bool wifi_hasCredentials(void)
{
    return spark::WiFi.hasCredentials();
}

bool wifi_clearCredentials(void)
{
    return spark::WiFi.clearCredentials();
}

void resolve(const char* name, uint32_t *ip)
{
    IPAddress IP;
    IP = spark::WiFi.resolve(name);
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

    spark::WiFi.setStaticIP(_host, _netmask, _gateway, _dns);
}

void wifi_useStaticIP(void)
{
	spark::WiFi.useStaticIP();
}

void wifi_useDynamicIP(void)
{
	spark::WiFi.useDynamicIP();
}

int wifi_scan(WiFiAccessPoint* results, size_t result_count)
{
    return spark::WiFi.scan(results, result_count);
}

int wifi_getCredentials(WiFiAccessPoint* results, size_t result_count)
{
    return spark::WiFi.getCredentials(results, result_count);
}


#ifdef __cplusplus
}
#endif
