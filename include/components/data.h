// Path: src/components/data.h
#ifndef __3D_SCANNER_DATA_H__
#define __3D_SCANNER_DATA_H__

#include <Arduino.h>

#include "nvs.h"
#include "esp_log.h"
#include "nvs_flash.h"

#define NVS_TAG_NAME "NVS"
#define NVS_STORAGE  "storage"

// STA WiFi

#define NVS_STA_SSID             "sta_ssid"
#define NVS_STA_PASSWORD         "sta_password"
#define NVS_STA_SSID_DEFAULT     "ssid"
#define NVS_STA_PASSWORD_DEFAULT "password"

// AP WiFi

#define NVS_AP_SSID             "ap_ssid"
#define NVS_AP_PASSWORD         "ap_password"
#define NVS_AP_SSID_DEFAULT     "3D Scanner"
#define NVS_AP_PASSWORD_DEFAULT "password"

// mDNS

#define NVS_MDNS_HOSTNAME         "mdns"
#define NVS_MDNS_HOSTNAME_DEFAULT "3d-scanner"

void init_nvs();

void set_sta_wifi(const char* ssid, const char* password);
void get_sta_wifi(char** ssid, char** password);

void set_ap_wifi(const char* ssid, const char* password);
void get_ap_wifi(char** ssid, char** password);

void set_mdns_hostname(const char* hostname);
void get_mdns_hostname(char** hostname);

#endif // __3D_SCANNER_DATA_H__
