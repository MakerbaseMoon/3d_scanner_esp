// Path: src/components/network.cpp
#include "components/network.h"    // include/components/network.h

const char* NETWORK_TAG = NETWORK_TAG_NAME;

char* sta_ssid = NULL;
char* sta_password = NULL;

char* ap_ssid = NULL;
char* ap_password = NULL;

char* hostname = NULL;

bool lost_ip = false;

IPAddress AP_local_ip(192, 168, 4, 1);
IPAddress AP_gateway(192, 168, 4, 1);
IPAddress AP_subnet(255, 255, 255, 0);

DNSServer dnsServer;

void set_ap_name(const char* name);

/**
 * @brief Set the AP name and connect to the AP
 * 
 * @param name `const char*`: the name of the AP
 */
void set_ap_name(const char* name) {
    WiFi.softAPdisconnect();
    WiFi.softAPConfig(AP_local_ip, AP_gateway, AP_subnet);

    if (ap_ssid != NULL && ap_password != NULL) {
        if (name == NULL || strlen(name) == 0) {
            ESP_LOGI(NETWORK_TAG, "Setting AP name to %s, password: %s", ap_ssid, ap_password);
            Serial.printf("AP %s, password: %s\n", ap_ssid, ap_password);
            WiFi.softAP(ap_ssid, ap_password);
        } else {
            char* new_ssid = (char*)malloc(strlen(ap_ssid) + strlen(name) + 1);
            sprintf(new_ssid, "%s-%s", ap_ssid, name);
            ESP_LOGI(NETWORK_TAG, "Setting AP name to %s, password: %s", new_ssid, ap_password);
            Serial.printf("AP %s, password: %s\n", new_ssid, ap_password);
            WiFi.softAP(new_ssid, ap_password);
        }
    }
}

/**
 * @brief Initialize the network
 * 
 */
void init_network() {
    WiFi.mode(WIFI_AP_STA);

    get_sta_wifi(&sta_ssid, &sta_password);
    get_ap_wifi(&ap_ssid, &ap_password);
    get_mdns_hostname(&hostname);

    if (hostname != NULL) {
        WiFi.setHostname(hostname);
        MDNS.begin(hostname);
    }

    if (sta_ssid != NULL && sta_password != NULL) {
        WiFi.begin(sta_ssid, sta_password);
    }

    WiFi.onEvent([](WiFiEvent_t event, WiFiEventInfo_t info) {
        switch (event) {
            case ARDUINO_EVENT_WIFI_STA_GOT_IP:
                Serial.printf("Connected to %s, IP address: %s\n", sta_ssid, WiFi.localIP().toString().c_str());
                lost_ip = false;
                set_ap_name(WiFi.localIP().toString().c_str());
                break;
            case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
                if (!lost_ip) {
                    lost_ip = true;
                    set_ap_name(NULL);
                }
                break;
            default:
                break;
        }
    });

    dnsServer.start(53, "*", AP_local_ip);
}
