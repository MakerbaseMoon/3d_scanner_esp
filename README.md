# 📦️ ESP32 3D Scanner

![Static Badge](https://img.shields.io/badge/framework-arduino-blue?logo=platformio)
![Static Badge](https://img.shields.io/badge/espressif32-6.8.1-blue?logo=espressif)
![GitHub Actions Workflow Status](https://img.shields.io/github/actions/workflow/status/MakerbaseMoon/3d_scanner_esp/platformIO.yml?label=OTA&logo=githubactions)
![GitHub Release](https://img.shields.io/github/v/release/MakerbaseMoon/3d_scanner_esp?logo=espressif&label=3D%20Scanner%20ESP32)
![GitHub Release](https://img.shields.io/github/v/release/MakerbaseMoon/3d_scanner_nextjs?logo=nextdotjs&label=3D%20Scanner%20Website)

This project is a 3D scanner based on the ESP32 microcontroller. It uses 2 stepper motor to move the VL53L1X sensor up and down. And use websocket to send the data to the [3D Scanner NextJS website client](https://github.com/MakerbaseMoon/3d_scanner_nextjs).

## 📚️ Table of Contents

- [📦️ ESP32 3D Scanner](#️-esp32-3d-scanner)
  - [📚️ Table of Contents](#️-table-of-contents)
  - [📦️ Hardware](#️-hardware)
  - [🔧️ Software](#️-software)
  - [🔧️ Usage](#️-usage)
  - [🔧️ WebServer API](#️-webserver-api)
  - [📦️ Libraries](#️-libraries)

## 📦️ Hardware

- ESP32
- 2x Stepper Motor
- 2x 1/32 Microstepping Driver
- VL53L1X Sensor
- SD Card Module

## 🔧️ Software

- [PlatformIO](https://platformio.org/)
- [CP210x USB to UART Bridge VCP Drivers](https://www.silabs.com/developers/usb-to-uart-bridge-vcp-drivers)
- [vscode](https://code.visualstudio.com/)
- [Github](https://github.com/)

## 🔧️ Usage

### Setting default WiFi Settings

platformio.ini

```ini
-DCONFIG_ESP_WIFI_SSID="My WiFi SSID"
-DCONFIG_ESP_WIFI_PASSWORD="My WiFi Password"
-DCONFIG_ESP_WIFI_AP_SSID="My ESP32 AP SSID"
-DCONFIG_ESP_WIFI_AP_PASSWORD="My ESP32 AP Password"
```

## 🔧️ WebServer API

See [WebServer API](https://github.com/MakerbaseMoon/3d_scanner_esp/blob/master/src/components/network.md)

## 📦️ Libraries

- [![Static Badge](https://img.shields.io/badge/ArduinoJson-7.1.0-orange)
](https://arduinojson.org/)
- [![Static Badge](https://img.shields.io/badge/ESPAsyncWebServer-latest-orange)
](https://github.com/me-no-dev/ESPAsyncWebServer)
- [![Static Badge](https://img.shields.io/badge/Adafruit%20VL53L1X-3.1.1-orange)
](https://github.com/adafruit/Adafruit_VL53L1X)
- [![Static Badge](https://img.shields.io/badge/ESP_Git_OTA-0.0.2-orange)
](https://registry.platformio.org/libraries/hsun/ESP_Git_OTA)

platformio.ini

```ini
lib_deps = 
    https://github.com/me-no-dev/ESPAsyncWebServer.git
    adafruit/Adafruit VL53L1X @ 3.1.1
    bblanchon/ArduinoJson @ 7.1.0
    hsun/ESP_Git_OTA @ 0.0.2
```
