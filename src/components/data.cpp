// Path: src/components/data.cpp
#include "components/data.h"    // include/components/data.h

const char* NVS_TAG = NVS_TAG_NAME;

/**
 * @brief Initialize the NVS flash memory
 * 
 */
void init_nvs() {
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK(err);
}

/**
 * @brief Set the STA WiFi credentials
 * 
 * @param ssid `const char*`: the SSID of the WiFi network 
 * @param password `const char*`: the password of the WiFi network
 */
void set_sta_wifi (const char* ssid, const char* password) {
    if (ssid == NULL || password == NULL) {
        ESP_LOGE(NVS_TAG, "STA ssid or password is NULL");
        return;
    }

    nvs_handle_t nvs_handle;
    esp_err_t err = nvs_open(NVS_STORAGE, NVS_READWRITE, &nvs_handle);
    if (err != ESP_OK) {
        ESP_LOGE(NVS_TAG, "Error (%s) opening STA NVS handle", esp_err_to_name(err));
    } else {
        err = nvs_set_str(nvs_handle, NVS_STA_SSID, ssid);
        if (err != ESP_OK) {
            ESP_LOGE(NVS_TAG, "Error (%s) writing STA ssid to NVS", esp_err_to_name(err));
        }
        err = nvs_set_str(nvs_handle, NVS_STA_PASSWORD, password);
        if (err != ESP_OK) {
            ESP_LOGE(NVS_TAG, "Error (%s) writing STA password to NVS", esp_err_to_name(err));
        }
        nvs_commit(nvs_handle);
        nvs_close(nvs_handle);
    }
}

/**
 * @brief Get the STA WiFi credentials
 * 
 * @param ssid `char**`: the SSID of the WiFi network, default is `ssid`  
 * @param password `char**`: the password of the WiFi network, default is `password`
 */
void get_sta_wifi(char** ssid, char** password) {
    if (*ssid != NULL ) free(*ssid);
    if (*password != NULL ) free(*password);

    nvs_handle_t nvs_handle;
    esp_err_t err = nvs_open(NVS_STORAGE, NVS_READONLY, &nvs_handle);
    if (err != ESP_OK) {
        ESP_LOGE(NVS_TAG, "Error (%s) opening STA NVS handle", esp_err_to_name(err));
    } else {
        size_t required_size;
        err = nvs_get_str(nvs_handle, NVS_STA_SSID, NULL, &required_size);
        if (err != ESP_OK) {
            ESP_LOGE(NVS_TAG, "Error (%s) reading STA ssid from NVS", esp_err_to_name(err));
        } else {
            *ssid = (char*)malloc(required_size);
            if (*ssid == NULL) {
                ESP_LOGE(NVS_TAG, "Error allocating memory for STA ssid");
            } else {
                err = nvs_get_str(nvs_handle, NVS_STA_SSID, *ssid, &required_size);
                if (err != ESP_OK) {
                    ESP_LOGE(NVS_TAG, "Error (%s) reading STA ssid from NVS", esp_err_to_name(err));
                }
            }
        }

        err = nvs_get_str(nvs_handle, NVS_STA_PASSWORD, NULL, &required_size);
        if (err != ESP_OK) {
            ESP_LOGE(NVS_TAG, "Error (%s) reading STA password from NVS", esp_err_to_name(err));
        } else {
            *password = (char*)malloc(required_size);
            if (*password == NULL) {
                ESP_LOGE(NVS_TAG, "Error allocating memory for STA password");
            } else {
                err = nvs_get_str(nvs_handle, NVS_STA_PASSWORD, *password, &required_size);
                if (err != ESP_OK) {
                    ESP_LOGE(NVS_TAG, "Error (%s) reading STA password from NVS", esp_err_to_name(err));
                }
            }
        }
        nvs_close(nvs_handle);
    }

    if (*ssid == NULL ) {
        *ssid = (char*)malloc(strlen(NVS_STA_SSID_DEFAULT) + 1);
        if (*ssid != NULL) {
            strcpy(*ssid, NVS_STA_SSID_DEFAULT);
        } else {
            ESP_LOGE(NVS_TAG, "Error allocating memory for STA ssid default");
        }
    }

    if (*password == NULL ) {
        *password = (char*)malloc(strlen(NVS_STA_PASSWORD_DEFAULT) + 1);
        if (*password != NULL) {
            strcpy(*password, NVS_STA_PASSWORD_DEFAULT);
        } else {
            ESP_LOGE(NVS_TAG, "Error allocating memory for STA password default");
        }
    }
}

/**
 * @brief Set the AP WiFi credentials
 * 
 * @param ssid `const char*`: the SSID of the WiFi network
 * @param password `const char*`: the password of the WiFi network
 */
void set_ap_wifi(const char* ssid, const char* password) {
    if (ssid == NULL || password == NULL) {
        ESP_LOGE(NVS_TAG, "AP ssid or password is NULL");
        return;
    }

    nvs_handle_t nvs_handle;
    esp_err_t err = nvs_open(NVS_STORAGE, NVS_READWRITE, &nvs_handle);
    if (err != ESP_OK) {
        ESP_LOGE(NVS_TAG, "Error (%s) opening AP NVS handle", esp_err_to_name(err));
    } else {
        err = nvs_set_str(nvs_handle, NVS_AP_SSID, ssid);
        if (err != ESP_OK) {
            ESP_LOGE(NVS_TAG, "Error (%s) writing AP ssid to NVS", esp_err_to_name(err));
        }
        err = nvs_set_str(nvs_handle, NVS_AP_PASSWORD, password);
        if (err != ESP_OK) {
            ESP_LOGE(NVS_TAG, "Error (%s) writing AP password to NVS", esp_err_to_name(err));
        }
        nvs_commit(nvs_handle);
        nvs_close(nvs_handle);
    }
}

/**
 * @brief Get the AP WiFi credentials
 * 
 * @param ssid `char**`: the SSID of the WiFi network, default is `3D Scanner`
 * @param password `char**`: the password of the WiFi network, default is `password`
 */
void get_ap_wifi(char** ssid, char** password) {
    if (*ssid != NULL ) free(*ssid);
    if (*password != NULL ) free(*password);

    nvs_handle_t nvs_handle;
    esp_err_t err = nvs_open(NVS_STORAGE, NVS_READONLY, &nvs_handle);
    if (err != ESP_OK) {
        ESP_LOGE(NVS_TAG, "Error (%s) opening AP NVS handle", esp_err_to_name(err));
    } else {
        size_t required_size;
        err = nvs_get_str(nvs_handle, NVS_AP_SSID, NULL, &required_size);
        if (err != ESP_OK) {
            ESP_LOGE(NVS_TAG, "Error (%s) reading AP ssid from NVS", esp_err_to_name(err));
        } else {
            *ssid = (char*)malloc(required_size);
            if (*ssid == NULL) {
                ESP_LOGE(NVS_TAG, "Error allocating memory for AP ssid");
            } else {
                err = nvs_get_str(nvs_handle, NVS_AP_SSID, *ssid, &required_size);
                if (err != ESP_OK) {
                    ESP_LOGE(NVS_TAG, "Error (%s) reading AP ssid from NVS", esp_err_to_name(err));
                }
            }
        }

        err = nvs_get_str(nvs_handle, NVS_AP_PASSWORD, NULL, &required_size);
        if (err != ESP_OK) {
            ESP_LOGE(NVS_TAG, "Error (%s) reading AP password from NVS", esp_err_to_name(err));
        } else {
            *password = (char*)malloc(required_size);
            if (*password == NULL) {
                ESP_LOGE(NVS_TAG, "Error allocating memory for AP password");
            } else {
                err = nvs_get_str(nvs_handle, NVS_AP_PASSWORD, *password, &required_size);
                if (err != ESP_OK) {
                    ESP_LOGE(NVS_TAG, "Error (%s) reading AP password from NVS", esp_err_to_name(err));
                }
            }
        }
        nvs_close(nvs_handle);
    }

    if (*ssid == NULL ) {
        *ssid = (char*)malloc(strlen(NVS_AP_SSID_DEFAULT) + 1);
        if (*ssid != NULL) {
            strcpy(*ssid, NVS_AP_SSID_DEFAULT);
        } else {
            ESP_LOGE(NVS_TAG, "Error allocating memory for AP ssid default");
        }
    }

    if (*password == NULL ) {
        *password = (char*)malloc(strlen(NVS_AP_PASSWORD_DEFAULT) + 1);
        if (*password != NULL) {
            strcpy(*password, NVS_AP_PASSWORD_DEFAULT);
        } else {
            ESP_LOGE(NVS_TAG, "Error allocating memory for AP password default");
        }
    }
}

/**
 * @brief Set the mDNS hostname
 * 
 * @param hostname `const char*`: the hostname
 */
void set_mdns_hostname(const char* hostname) {
    if (hostname == NULL) {
        ESP_LOGE(NVS_TAG, "mDNS hostname is NULL");
        return;
    }

    nvs_handle_t nvs_handle;
    esp_err_t err = nvs_open(NVS_STORAGE, NVS_READWRITE, &nvs_handle);
    if (err != ESP_OK) {
        ESP_LOGE(NVS_TAG, "Error (%s) opening mDNS NVS handle", esp_err_to_name(err));
    } else {
        err = nvs_set_str(nvs_handle, NVS_MDNS_HOSTNAME, hostname);
        if (err != ESP_OK) {
            ESP_LOGE(NVS_TAG, "Error (%s) writing mDNS hostname to NVS", esp_err_to_name(err));
        }
        nvs_commit(nvs_handle);
        nvs_close(nvs_handle);
    }
}

/**
 * @brief Get the mDNS hostname
 * 
 * @param hostname `char**`: the hostname, default is `3d-scanner`
 */
void get_mdns_hostname(char** hostname) {
    if (*hostname != NULL ) free(*hostname);

    nvs_handle_t nvs_handle;
    esp_err_t err = nvs_open(NVS_STORAGE, NVS_READONLY, &nvs_handle);
    if (err != ESP_OK) {
        ESP_LOGE(NVS_TAG, "Error (%s) opening mDNS NVS handle", esp_err_to_name(err));
    } else {
        size_t required_size;
        err = nvs_get_str(nvs_handle, NVS_MDNS_HOSTNAME, NULL, &required_size);
        if (err != ESP_OK) {
            ESP_LOGE(NVS_TAG, "Error (%s) reading mDNS hostname from NVS", esp_err_to_name(err));
        } else {
            *hostname = (char*)malloc(required_size);
            if (*hostname == NULL) {
                ESP_LOGE(NVS_TAG, "Error allocating memory for mDNS hostname");
            } else {
                err = nvs_get_str(nvs_handle, NVS_MDNS_HOSTNAME, *hostname, &required_size);
                if (err != ESP_OK) {
                    ESP_LOGE(NVS_TAG, "Error (%s) reading mDNS hostname from NVS", esp_err_to_name(err));
                }
            }
        }
        nvs_close(nvs_handle);
    }

    if (*hostname == NULL ) {
        *hostname = (char*)malloc(strlen(NVS_MDNS_HOSTNAME_DEFAULT) + 1);
        if (*hostname != NULL) {
            strcpy(*hostname, NVS_MDNS_HOSTNAME_DEFAULT);
        } else {
            ESP_LOGE(NVS_TAG, "Error allocating memory for mDNS hostname default");
        }
    }
}
