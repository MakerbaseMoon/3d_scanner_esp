// Path: src/components/ota.h
#ifndef __3D_SCANNER_OTA_H__
#define __3D_SCANNER_OTA_H__

#include <Arduino.h>

#include "esp_log.h"

#include <GithubReleaseOTA.h>

#define OTA_TAG_NAME "OTA"

void flash_firmware(const char* username, const char* repo, int asset_id);
void ota_loop();

#endif // __3D_SCANNER_OTA_H__
