// Path: include/components/server.h
#ifndef __3D_SCANNER_SERVER_H__
#define __3D_SCANNER_SERVER_H__

#include <Arduino.h>

#include <AsyncWebSocket.h>
#include <ESPAsyncWebServer.h>

#include "components/data.h"

void init_server();
void ws_send_text(const char* message);

#endif // __3D_SCANNER_SERVER_H__