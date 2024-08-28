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
            if (err == ESP_ERR_NVS_NOT_FOUND) ESP_LOGW(NVS_TAG, "Error (%s) reading STA ssid from NVS", esp_err_to_name(err));
            else ESP_LOGE(NVS_TAG, "Error (%s) reading STA ssid from NVS", esp_err_to_name(err));
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
            if (err == ESP_ERR_NVS_NOT_FOUND) ESP_LOGW(NVS_TAG, "Error (%s) reading STA password from NVS", esp_err_to_name(err));
            else ESP_LOGE(NVS_TAG, "Error (%s) reading STA password from NVS", esp_err_to_name(err));
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
            if (err == ESP_ERR_NVS_NOT_FOUND) ESP_LOGW(NVS_TAG, "Error (%s) reading AP ssid from NVS", esp_err_to_name(err));
            else ESP_LOGE(NVS_TAG, "Error (%s) reading AP ssid from NVS", esp_err_to_name(err));
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
            if (err == ESP_ERR_NVS_NOT_FOUND) ESP_LOGW(NVS_TAG, "Error (%s) reading AP password from NVS", esp_err_to_name(err));
            else ESP_LOGE(NVS_TAG, "Error (%s) reading AP password from NVS", esp_err_to_name(err));
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
            if (err == ESP_ERR_NVS_NOT_FOUND) ESP_LOGW(NVS_TAG, "Error (%s) reading mDNS hostname from NVS", esp_err_to_name(err));
            else ESP_LOGE(NVS_TAG, "Error (%s) reading mDNS hostname from NVS", esp_err_to_name(err));
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


void set_github(const char* username, const char* repo) {
    if (username == NULL || repo == NULL) {
        ESP_LOGE(NVS_TAG, "Github username or repo is NULL");
        return;
    }

    nvs_handle_t nvs_handle;
    esp_err_t err = nvs_open(NVS_STORAGE, NVS_READWRITE, &nvs_handle);
    if (err != ESP_OK) {
        ESP_LOGE(NVS_TAG, "Error (%s) opening Github NVS handle", esp_err_to_name(err));
    } else {
        err = nvs_set_str(nvs_handle, NVS_GITHUB_USERNAME, username);
        if (err != ESP_OK) {
            ESP_LOGE(NVS_TAG, "Error (%s) writing Github username to NVS", esp_err_to_name(err));
        }
        err = nvs_set_str(nvs_handle, NVS_GITHUB_REPO, repo);
        if (err != ESP_OK) {
            ESP_LOGE(NVS_TAG, "Error (%s) writing Github repo to NVS", esp_err_to_name(err));
        }
        nvs_commit(nvs_handle);
        nvs_close(nvs_handle);
    }
}

void get_github(char** username, char** repo) {
    if (*username != NULL ) free(*username);
    if (*repo != NULL ) free(*repo);

    nvs_handle_t nvs_handle;
    esp_err_t err = nvs_open(NVS_STORAGE, NVS_READONLY, &nvs_handle);
    if (err != ESP_OK) {
        ESP_LOGE(NVS_TAG, "Error (%s) opening Github NVS handle", esp_err_to_name(err));
    } else {
        size_t required_size;
        err = nvs_get_str(nvs_handle, NVS_GITHUB_USERNAME, NULL, &required_size);
        if (err != ESP_OK) {
            if (err == ESP_ERR_NVS_NOT_FOUND) ESP_LOGW(NVS_TAG, "Error (%s) reading Github username from NVS", esp_err_to_name(err));
            else ESP_LOGE(NVS_TAG, "Error (%s) reading Github username from NVS", esp_err_to_name(err));
        } else {
            *username = (char*)malloc(required_size);
            if (*username == NULL) {
                ESP_LOGE(NVS_TAG, "Error allocating memory for Github username");
            } else {
                err = nvs_get_str(nvs_handle, NVS_GITHUB_USERNAME, *username, &required_size);
                if (err != ESP_OK) {
                    ESP_LOGE(NVS_TAG, "Error (%s) reading Github username from NVS", esp_err_to_name(err));
                }
            }
        }

        err = nvs_get_str(nvs_handle, NVS_GITHUB_REPO, NULL, &required_size);
        if (err != ESP_OK) {
            if (err == ESP_ERR_NVS_NOT_FOUND) ESP_LOGW(NVS_TAG, "Error (%s) reading Github repo from NVS", esp_err_to_name(err));
            else ESP_LOGE(NVS_TAG, "Error (%s) reading Github repo from NVS", esp_err_to_name(err));
        } else {
            *repo = (char*)malloc(required_size);
            if (*repo == NULL) {
                ESP_LOGE(NVS_TAG, "Error allocating memory for Github repo");
            } else {
                err = nvs_get_str(nvs_handle, NVS_GITHUB_REPO, *repo, &required_size);
                if (err != ESP_OK) {
                    ESP_LOGE(NVS_TAG, "Error (%s) reading Github repo from NVS", esp_err_to_name(err));
                }
            }
        }
        nvs_close(nvs_handle);
    }

    if (*username == NULL ) {
        *username = (char*)malloc(strlen(NVS_GITHUB_USERNAME_DEFAULT) + 1);
        if (*username != NULL) {
            strcpy(*username, NVS_GITHUB_USERNAME_DEFAULT);
        } else {
            ESP_LOGE(NVS_TAG, "Error allocating memory for Github username default");
        }
    }

    if (*repo == NULL ) {
        *repo = (char*)malloc(strlen(NVS_GITHUB_REPO_DEFAULT) + 1);
        if (*repo != NULL) {
            strcpy(*repo, NVS_GITHUB_REPO_DEFAULT);
        } else {
            ESP_LOGE(NVS_TAG, "Error allocating memory for Github repo default");
        }
    }
}

void set_module(uint16_t z_axis_max, uint16_t z_axis_start_step, uint16_t z_axis_delay_time, uint16_t z_axis_one_time_step, 
                uint16_t x_y_axis_max, uint16_t x_y_axis_check_times, uint16_t x_y_axis_step_delay_time, uint16_t x_y_axis_one_time_step,
                uint16_t vl53l1x_center, uint16_t vl53l1x_timeing_budget) {
    nvs_handle_t nvs_handle;
    esp_err_t err = nvs_open(NVS_STORAGE, NVS_READWRITE, &nvs_handle);
    if (err != ESP_OK) {
        ESP_LOGE(NVS_TAG, "Error (%s) opening Module NVS handle", esp_err_to_name(err));
    } else {
        err = nvs_set_u16(nvs_handle, NVS_Z_AXIS_MAX, z_axis_max);
        if (err != ESP_OK) {
            ESP_LOGE(NVS_TAG, "Error (%s) writing Z axis max to NVS", esp_err_to_name(err));
        }
        err = nvs_set_u16(nvs_handle, NVS_Z_AXIS_START_STEP, z_axis_start_step);
        if (err != ESP_OK) {
            ESP_LOGE(NVS_TAG, "Error (%s) writing Z axis start step to NVS", esp_err_to_name(err));
        }
        err = nvs_set_u16(nvs_handle, NVS_Z_AXIS_DELAY_TIME, z_axis_delay_time);
        if (err != ESP_OK) {
            ESP_LOGE(NVS_TAG, "Error (%s) writing Z axis delay time to NVS", esp_err_to_name(err));
        }
        err = nvs_set_u16(nvs_handle, NVS_Z_AXIS_ONE_TIME_STEP, z_axis_one_time_step);
        if (err != ESP_OK) {
            ESP_LOGE(NVS_TAG, "Error (%s) writing Z axis one time step to NVS", esp_err_to_name(err));
        }
        err = nvs_set_u16(nvs_handle, NVS_X_Y_AXIS_MAX, x_y_axis_max);
        if (err != ESP_OK) {
            ESP_LOGE(NVS_TAG, "Error (%s) writing X Y axis max to NVS", esp_err_to_name(err));
        }
        err = nvs_set_u16(nvs_handle, NVS_X_Y_AXIS_CHECK_TIMES, x_y_axis_check_times);
        if (err != ESP_OK) {
            ESP_LOGE(NVS_TAG, "Error (%s) writing X Y axis check times to NVS", esp_err_to_name(err));
        }
        err = nvs_set_u16(nvs_handle, NVS_X_Y_AXIS_STEP_DELAY_TIME, x_y_axis_step_delay_time);
        if (err != ESP_OK) {
            ESP_LOGE(NVS_TAG, "Error (%s) writing X Y axis step delay time to NVS", esp_err_to_name(err));
        }
        err = nvs_set_u16(nvs_handle, NVS_X_Y_AXIS_ONE_TIME_STEP, x_y_axis_one_time_step);
        if (err != ESP_OK) {
            ESP_LOGE(NVS_TAG, "Error (%s) writing X Y axis one time step to NVS", esp_err_to_name(err));
        }
        err = nvs_set_u16(nvs_handle, NVS_VL53L1X_CENTER, vl53l1x_center);
        if (err != ESP_OK) {
            ESP_LOGE(NVS_TAG, "Error (%s) writing VL53L1X center to NVS", esp_err_to_name(err));
        }
        err = nvs_set_u16(nvs_handle, NVS_VL53L1X_TIMEING_BUDGET, vl53l1x_timeing_budget);
        if (err != ESP_OK) {
            ESP_LOGE(NVS_TAG, "Error (%s) writing VL53L1X timeing budget to NVS", esp_err_to_name(err));
        }
        nvs_commit(nvs_handle);
        nvs_close(nvs_handle);
    }
}

void get_module(uint16_t* z_axis_max, uint16_t* z_axis_start_step, uint16_t* z_axis_delay_time, uint16_t* z_axis_one_time_step, 
                uint16_t* x_y_axis_max, uint16_t* x_y_axis_check_times, uint16_t* x_y_axis_step_delay_time, uint16_t* x_y_axis_one_time_step,
                uint16_t* vl53l1x_center, uint16_t* vl53l1x_timeing_budget) {
    nvs_handle_t nvs_handle;
    esp_err_t err = nvs_open(NVS_STORAGE, NVS_READONLY, &nvs_handle);
    if (err != ESP_OK) {
        ESP_LOGE(NVS_TAG, "Error (%s) opening Module NVS handle", esp_err_to_name(err));
    } else {
        err = nvs_get_u16(nvs_handle, NVS_Z_AXIS_MAX, z_axis_max);
        if (err != ESP_OK) {
            if (err == ESP_ERR_NVS_NOT_FOUND) ESP_LOGW(NVS_TAG, "Error (%s) reading Z axis max from NVS", esp_err_to_name(err));
            else ESP_LOGE(NVS_TAG, "Error (%s) reading Z axis max from NVS", esp_err_to_name(err));
            *z_axis_max = NVS_Z_AXIS_MAX_DEFAULT;
        }
        err = nvs_get_u16(nvs_handle, NVS_Z_AXIS_START_STEP, z_axis_start_step);
        if (err != ESP_OK) {
            if (err == ESP_ERR_NVS_NOT_FOUND) ESP_LOGW(NVS_TAG, "Error (%s) reading Z axis start step from NVS", esp_err_to_name(err));
            else ESP_LOGE(NVS_TAG, "Error (%s) reading Z axis start step from NVS", esp_err_to_name(err));
            *z_axis_start_step = NVS_Z_AXIS_START_STEP_DEFAULT;
        }
        err = nvs_get_u16(nvs_handle, NVS_Z_AXIS_DELAY_TIME, z_axis_delay_time);
        if (err != ESP_OK) {
            if (err == ESP_ERR_NVS_NOT_FOUND) ESP_LOGW(NVS_TAG, "Error (%s) reading Z axis delay time from NVS", esp_err_to_name(err));
            else ESP_LOGE(NVS_TAG, "Error (%s) reading Z axis delay time from NVS", esp_err_to_name(err));
            *z_axis_delay_time = NVS_Z_AXIS_DELAY_TIME_DEFAULT;
        }
        err = nvs_get_u16(nvs_handle, NVS_Z_AXIS_ONE_TIME_STEP, z_axis_one_time_step);
        if (err != ESP_OK) {
            if (err == ESP_ERR_NVS_NOT_FOUND) ESP_LOGW(NVS_TAG, "Error (%s) reading Z axis one time step from NVS", esp_err_to_name(err));
            else ESP_LOGE(NVS_TAG, "Error (%s) reading Z axis one time step from NVS", esp_err_to_name(err));
            *z_axis_one_time_step = NVS_Z_AXIS_ONE_TIME_STEP_DEFAULT;
        }
        err = nvs_get_u16(nvs_handle, NVS_X_Y_AXIS_MAX, x_y_axis_max);
        if (err != ESP_OK) {
            if (err == ESP_ERR_NVS_NOT_FOUND) ESP_LOGW(NVS_TAG, "Error (%s) reading X Y axis max from NVS", esp_err_to_name(err));
            else ESP_LOGE(NVS_TAG, "Error (%s) reading X Y axis max from NVS", esp_err_to_name(err));
            *x_y_axis_max = NVS_X_Y_AXIS_MAX_DEFAULT;
        }
        err = nvs_get_u16(nvs_handle, NVS_X_Y_AXIS_CHECK_TIMES, x_y_axis_check_times);
        if (err != ESP_OK) {
            if (err == ESP_ERR_NVS_NOT_FOUND) ESP_LOGW(NVS_TAG, "Error (%s) reading X Y axis check times from NVS", esp_err_to_name(err));
            else ESP_LOGE(NVS_TAG, "Error (%s) reading X Y axis check times from NVS", esp_err_to_name(err));
            *x_y_axis_check_times = NVS_X_Y_AXIS_CHECK_TIMES_DEFAULT;
        }
        err = nvs_get_u16(nvs_handle, NVS_X_Y_AXIS_STEP_DELAY_TIME, x_y_axis_step_delay_time);
        if (err != ESP_OK) {
            if (err == ESP_ERR_NVS_NOT_FOUND) ESP_LOGW(NVS_TAG, "Error (%s) reading X Y axis step delay time from NVS", esp_err_to_name(err));
            else ESP_LOGE(NVS_TAG, "Error (%s) reading X Y axis step delay time from NVS", esp_err_to_name(err));
            *x_y_axis_step_delay_time = NVS_X_Y_AXIS_STEP_DELAY_TIME_DEFAULT;
        }
        err = nvs_get_u16(nvs_handle, NVS_X_Y_AXIS_ONE_TIME_STEP, x_y_axis_one_time_step);
        if (err != ESP_OK) {
            if (err == ESP_ERR_NVS_NOT_FOUND) ESP_LOGW(NVS_TAG, "Error (%s) reading X Y axis one time step from NVS", esp_err_to_name(err));
            else ESP_LOGE(NVS_TAG, "Error (%s) reading X Y axis one time step from NVS", esp_err_to_name(err));
            *x_y_axis_one_time_step = NVS_X_Y_AXIS_ONE_TIME_STEP_DEFAULT;
        }
        err = nvs_get_u16(nvs_handle, NVS_VL53L1X_CENTER, vl53l1x_center);
        if (err != ESP_OK) {
            if (err == ESP_ERR_NVS_NOT_FOUND) ESP_LOGW(NVS_TAG, "Error (%s) reading VL53L1X center from NVS", esp_err_to_name(err));
            else ESP_LOGE(NVS_TAG, "Error (%s) reading VL53L1X center from NVS", esp_err_to_name(err));
            *vl53l1x_center = NVS_VL53L1X_CENTER_DEFAULT;
        }
        err = nvs_get_u16(nvs_handle, NVS_VL53L1X_TIMEING_BUDGET, vl53l1x_timeing_budget);
        if (err != ESP_OK) {
            if (err == ESP_ERR_NVS_NOT_FOUND) ESP_LOGW(NVS_TAG, "Error (%s) reading VL53L1X timeing budget from NVS", esp_err_to_name(err));
            else ESP_LOGE(NVS_TAG, "Error (%s) reading VL53L1X timeing budget from NVS", esp_err_to_name(err));
            *vl53l1x_timeing_budget = NVS_VL53L1X_TIMEING_BUDGET_DEFAULT;
        }
        nvs_close(nvs_handle);
    }
}
