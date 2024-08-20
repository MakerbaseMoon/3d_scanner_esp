// Path: src/components/ota.cpp
#include "components/ota.h"    // include/components/ota.h

const char* OTA_TAG = OTA_TAG_NAME;

int _asset_id;
String _username;
String _repo;

bool is_ota = false;

void flash_firmware(const char* username, const char* repo, int asset_id) {
    _username = username;
    _repo = repo;
    _asset_id = asset_id;

    is_ota = true;
}

void ota_loop() {
    if(is_ota) {
        GithubReleaseOTA ota(_username.c_str(), _repo.c_str());
        int result = ota.flashByAssetId(_asset_id, U_FLASH);
        if (result == 0) {
            ESP_LOGI(OTA_TAG, "OTA success");
        } else {
            ESP_LOGE(OTA_TAG, "OTA failed code: %d", result);
        }
    }
}
