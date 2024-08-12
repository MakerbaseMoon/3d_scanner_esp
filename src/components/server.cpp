// Path: src/components/server.cpp
#include "components/server.h"    // include/components/server.h

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len);

void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
    switch(type) {
        case WS_EVT_CONNECT:
            Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
            break;
        case WS_EVT_DISCONNECT:
            Serial.printf("WebSocket client #%u disconnected\n", client->id());
            break;
        case WS_EVT_DATA:
            AwsFrameInfo *info = (AwsFrameInfo*)arg;
            if(info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
                data[len] = 0;
                Serial.printf("WebSocket client #%u message: %s\n", client->id(), (char*)data);
            }
            break;
    }
}

void init_server() {
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(200, "text/plain", "Hello, world!");
    });

    server.on("/api/info", HTTP_GET, [](AsyncWebServerRequest *request) {
        char* ssid = NULL;
        char* password = NULL;
        char* ap_ssid = NULL;
        char* ap_password = NULL;
        char* hostname = NULL;

        get_sta_wifi(&ssid, &password);
        get_ap_wifi(&ap_ssid, &ap_password);
        get_mdns_hostname(&hostname);

        if (ssid != NULL && password != NULL && ap_ssid != NULL && ap_password != NULL && hostname != NULL) {
            String data = "{\"code\": 200,\"status\": \"ok\",\"path\": \"/api/info\",\"data\": {\"sta\": {\"ssid\": \"" + String(ssid) + "\",\"password\": \"" + String(password) + "\"},\"ap\": {\"ssid\": \"" + String(ap_ssid) + "\",\"password\": \"" + String(ap_password) + "\"},\"mdns\": {\"hostname\": \"" + String(hostname) + "\"}}}";
            request->send(200, "application/json", data);
        } else {
            request->send(500, "application/json", "{\"code\": 500,\"status\": \"Server Error\",\"path\": \"/api/info\"}");
        } 
        
        if (ssid != NULL) free(ssid);
        if (password != NULL) free(password);

        if (ap_ssid != NULL) free(ap_ssid);
        if (ap_password != NULL) free(ap_password);

        if (hostname != NULL) free(hostname);
    });

    server.on("/api/set/sta", HTTP_GET, [](AsyncWebServerRequest *request) {
        String ssid = request->getParam("ssid", true)->value();
        String password = request->getParam("password", true)->value();
        if (ssid.length() > 0 && password.length() > 0) {
            set_sta_wifi(ssid.c_str(), password.c_str());
            request->send(200, "application/json", "{\"code\": 200,\"status\": \"ok\",\"path\": \"/api/set/sta\"}");
        } else {
            request->send(400, "application/json", "{\"code\": 400,\"status\": \"error\",\"path\": \"/api/set/sta\"}");
        }
    });

    server.on("/api/set/ap", HTTP_GET, [](AsyncWebServerRequest *request) {
        String ssid = request->getParam("ssid", true)->value();
        String password = request->getParam("password", true)->value();
        if (ssid.length() > 0 && password.length() > 0) {
            set_sta_wifi(ssid.c_str(), password.c_str());
            request->send(200, "application/json", "{\"code\": 200,\"status\": \"ok\",\"path\": \"/api/set/ap\"}");
        } else {
            request->send(400, "application/json", "{\"code\": 400,\"status\": \"error\",\"path\": \"/api/set/ap\"}");
        }
    });

    server.on("/api/set/mdns", HTTP_GET, [](AsyncWebServerRequest *request) {
        String hostname = request->getParam("hostname", true)->value();
        if (hostname.length() > 0) {
            set_mdns_hostname(hostname.c_str());
            request->send(200, "application/json", "{\"code\": 200,\"status\": \"ok\",\"path\": \"/api/set/mdns\"}");
        } else {
            request->send(400, "application/json", "{\"code\": 400,\"status\": \"error\",\"path\": \"/api/set/mdns\"}");
        }
    });

    server.on("/api/set/scanner", HTTP_GET, [](AsyncWebServerRequest *request) {
        String status = request->getParam("status", true)->value();
        Serial.println(status);
    });

    server.onNotFound([](AsyncWebServerRequest *request){
        request->send(404, "text/plain", "404 NOT Found!");
    });

    ws.onEvent(onEvent);

    server.addHandler(&ws);
    server.begin();
}

void ws_send_text(const char* message) {
    ws.textAll(message);
}