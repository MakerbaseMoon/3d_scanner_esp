// Path: include/components/server.h
#ifndef __3D_SCANNER_SERVER_H__
#define __3D_SCANNER_SERVER_H__

#include <Arduino.h>

#include <AsyncJson.h>
#include <ArduinoJson.h>
#include <AsyncWebSocket.h>
#include <ESPAsyncWebServer.h>

#include "components/ota.h"
#include "components/data.h"
#include "components/module.h"
#include "version.h"
#include "website.h"

#define SERVER_TAG_NAME "server"

void init_server();
void ws_send_text(const char* message);

#endif // __3D_SCANNER_SERVER_H__