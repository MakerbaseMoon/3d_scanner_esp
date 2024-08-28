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

#define NVS_STA_SSID             "SS"
#define NVS_STA_PASSWORD         "SP"
#ifdef CONFIG_ESP_WIFI_SSID
#define NVS_STA_SSID_DEFAULT     CONFIG_ESP_WIFI_SSID
#else
#define NVS_STA_SSID_DEFAULT     "ssid"
#endif
#ifdef CONFIG_ESP_WIFI_PASSWORD
#define NVS_STA_PASSWORD_DEFAULT CONFIG_ESP_WIFI_PASSWORD
#else
#define NVS_STA_PASSWORD_DEFAULT "password"
#endif

// AP WiFi

#define NVS_AP_SSID             "AS"
#define NVS_AP_PASSWORD         "AP"
#ifdef CONFIG_ESP_WIFI_AP_SSID
#define NVS_AP_SSID_DEFAULT     CONFIG_ESP_WIFI_AP_SSID
#else
#define NVS_AP_SSID_DEFAULT     "3D Scanner"
#endif
#ifdef CONFIG_ESP_WIFI_AP_PASSWORD
#define NVS_AP_PASSWORD_DEFAULT CONFIG_ESP_WIFI_AP_PASSWORD
#else
#define NVS_AP_PASSWORD_DEFAULT "password"
#endif

// mDNS

#define NVS_MDNS_HOSTNAME         "M"
#define NVS_MDNS_HOSTNAME_DEFAULT "3d-scanner"

// Github
#define NVS_GITHUB_REPO             "GR"
#define NVS_GITHUB_USERNAME         "GU"
#define NVS_GITHUB_REPO_DEFAULT     "3d_scanner_esp"
#define NVS_GITHUB_USERNAME_DEFAULT "MakerbaseMoon"

// Module

#define NVS_Z_AXIS_MAX           "ZM"
#define NVS_Z_AXIS_START_STEP    "ZSS"
#define NVS_Z_AXIS_DELAY_TIME    "ZDT"
#define NVS_Z_AXIS_ONE_TIME_STEP "ZOTS"

#define NVS_Z_AXIS_MAX_DEFAULT          47000
#define NVS_Z_AXIS_START_STEP_DEFAULT       0
#define NVS_Z_AXIS_DELAY_TIME_DEFAULT     100
#define NVS_Z_AXIS_ONE_TIME_STEP_DEFAULT  400

#define NVS_X_Y_AXIS_MAX             "2M"
#define NVS_X_Y_AXIS_CHECK_TIMES     "2CT"
#define NVS_X_Y_AXIS_STEP_DELAY_TIME "2STD"
#define NVS_X_Y_AXIS_ONE_TIME_STEP   "2OTS"

#define NVS_X_Y_AXIS_MAX_DEFAULT             6400
#define NVS_X_Y_AXIS_CHECK_TIMES_DEFAULT        1
#define NVS_X_Y_AXIS_STEP_DELAY_TIME_DEFAULT   50
#define NVS_X_Y_AXIS_ONE_TIME_STEP_DEFAULT      8

#define NVS_VL53L1X_CENTER         "R_C"
#define NVS_VL53L1X_TIMEING_BUDGET "R_TB"

#define NVS_VL53L1X_CENTER_DEFAULT            70
// Valid timing budgets: 15, 20, 33, 50, 100, 200 and 500ms!
#define NVS_VL53L1X_TIMEING_BUDGET_DEFAULT    200 

// Functions 

void init_nvs();

void set_sta_wifi(const char* ssid, const char* password);
void get_sta_wifi(char** ssid, char** password);

void set_ap_wifi(const char* ssid, const char* password);
void get_ap_wifi(char** ssid, char** password);

void set_mdns_hostname(const char* hostname);
void get_mdns_hostname(char** hostname);

void set_github(const char* username, const char* repo);
void get_github(char** username, char** repo);

void set_module(uint16_t z_axis_max, uint16_t z_axis_start_step, uint16_t z_axis_delay_time, uint16_t z_axis_one_time_step, 
                uint16_t x_y_axis_max, uint16_t x_y_axis_check_times, uint16_t x_y_axis_step_delay_time, uint16_t x_y_axis_one_time_step,
                uint16_t vl53l1x_center, uint16_t vl53l1x_timeing_budget);
void get_module(uint16_t* z_axis_max, uint16_t* z_axis_start_step, uint16_t* z_axis_delay_time, uint16_t* z_axis_one_time_step, 
                uint16_t* x_y_axis_max, uint16_t* x_y_axis_check_times, uint16_t* x_y_axis_step_delay_time, uint16_t* x_y_axis_one_time_step,
                uint16_t* vl53l1x_center, uint16_t* vl53l1x_timeing_budget);

#endif // __3D_SCANNER_DATA_H__
