// Path: src/components/server.cpp
#include "components/server.h"    // include/components/server.h

const char* SERVER_TAG = SERVER_TAG_NAME;

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

void message(const char* message);
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
                message((char*)data);
            }
            break;
    }
}

void message(const char* message) {
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, message);
    if (error) {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.f_str());
        return;
    }

    if(!doc["command"].isNull()) {
        if (doc["command"] == "home") {
            set_command(SCANNER_COMMAND_HOME);
        } else if (doc["command"] == "new") {
            if (!doc["name"].isNull()) {
                set_project_name(doc["name"]);
                set_command(SCANNER_COMMAND_START);
            }
        } else if (doc["command"] == "start") {
            set_command(SCANNER_COMMAND_START);
        } else if (doc["command"] == "stop") {
            set_command(SCANNER_COMMAND_STOP);
        } else if (doc["command"] == "up") {
            if (!doc["step"].isNull()) {
                set_command(SCANNER_COMMAND_UP, doc["step"]);
            } else {
                set_command(SCANNER_COMMAND_UP, 1);
            }
        } else if (doc["command"] == "down") {
            if (!doc["step"].isNull()) {
                set_command(SCANNER_COMMAND_DOWN, doc["step"]);
            } else {
                set_command(SCANNER_COMMAND_DOWN, 1);
            }
        } else if (doc["command"] == "right") {
            if (!doc["step"].isNull()) {
                set_command(SCANNER_COMMAND_RIGHT, doc["step"]);
            } else {
                set_command(SCANNER_COMMAND_RIGHT, 1);
            }
        } else if (doc["command"] == "left") {
            if (!doc["step"].isNull()) {
                set_command(SCANNER_COMMAND_LEFT, doc["step"]);
            } else {
                set_command(SCANNER_COMMAND_LEFT, 1);
            }
        }
    }

    doc.clear();
}

void init_server() {
    if(!SPIFFS.begin(true)){
        Serial.println("An Error has occurred while mounting SPIFFS");
        return;
    }
    
    DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");

    server.serveStatic("/", SPIFFS, "/").setDefaultFile("index.html");

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
            JsonDocument doc;
            doc["code"] = 200;
            doc["status"] = "ok";
            doc["path"] = "/api/info";
            JsonObject data = doc.createNestedObject("data");
            JsonObject sta = data.createNestedObject("sta");
            sta["ssid"] = ssid;
            sta["password"] = password;
            JsonObject ap = data.createNestedObject("ap");
            ap["ssid"] = ap_ssid;
            ap["password"] = ap_password;
            JsonObject mdns = data.createNestedObject("mdns");
            mdns["hostname"] = hostname;
            
            String response;
            serializeJson(doc, response);
            request->send(200, "application/json", response);
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
        int code = 200;
        String status = "ok";
        try {
            if (request->getParam("command")) {
                String command = request->getParam("command")->value();
                if(command == "home") {
                    ESP_LOGD(SERVER_TAG, "Home command");
                    set_command(SCANNER_COMMAND_HOME);
                } else if(command == "new") {
                    if (request->getParam("name") != NULL) {
                        ESP_LOGD(SERVER_TAG, "New command, name: %s", request->getParam("name")->value().c_str());
                        set_project_name(request->getParam("name")->value().c_str());
                        set_command(SCANNER_COMMAND_START);
                    } else {
                        code = 400;
                        status = "param not found";
                    }
                } else if(command == "start") {
                    ESP_LOGD(SERVER_TAG, "Start command");
                    set_command(SCANNER_COMMAND_START);
                } else if(command == "stop") {
                    ESP_LOGD(SERVER_TAG, "Stop command");
                    set_command(SCANNER_COMMAND_STOP);
                } else if(command == "up") {
                    if (request->getParam("step") != NULL) {
                        ESP_LOGD(SERVER_TAG, "Up command, step: %d", request->getParam("step")->value().toInt());
                        set_command(SCANNER_COMMAND_UP, request->getParam("step")->value().toInt());
                    } else {
                        ESP_LOGD(SERVER_TAG, "Up command, step: 1");
                        set_command(SCANNER_COMMAND_UP, 1);
                    }
                } else if(command == "down") {
                    if (request->getParam("step") != NULL) {
                        ESP_LOGD(SERVER_TAG, "Down command, step: %d", request->getParam("step")->value().toInt());
                        set_command(SCANNER_COMMAND_DOWN, request->getParam("step")->value().toInt());
                    } else {
                        ESP_LOGD(SERVER_TAG, "Down command, step: 1");
                        set_command(SCANNER_COMMAND_DOWN, 1);
                    }
                } else if(command == "right") {
                    if (request->getParam("step") != NULL) {
                        ESP_LOGD(SERVER_TAG, "Right command, step: %d", request->getParam("step")->value().toInt());
                        set_command(SCANNER_COMMAND_RIGHT, request->getParam("step")->value().toInt());
                    } else {
                        ESP_LOGD(SERVER_TAG, "Right command, step: 1");
                        set_command(SCANNER_COMMAND_RIGHT, 1);
                    }
                } else if(command == "left") {
                    if (request->getParam("step") != NULL) {
                        ESP_LOGD(SERVER_TAG, "Left command, step: %d", request->getParam("step")->value().toInt());
                        set_command(SCANNER_COMMAND_LEFT, request->getParam("step")->value().toInt());
                    } else {
                        ESP_LOGD(SERVER_TAG, "Left command, step: 1");
                        set_command(SCANNER_COMMAND_LEFT, 1);
                    }
                } else {
                    ESP_LOGW(SERVER_TAG, "Command not found");
                    code = 400;
                    status = "command not found";
                }
            } else {
                ESP_LOGW(SERVER_TAG, "Param not found");
                code = 400;
                status = "param not found";
            }
        }
        catch(const std::exception& e) {
            ESP_LOGE(SERVER_TAG, "Error: %s", e.what());
            code = 500;
            status = e.what();
        }

        request->send(code, "application/json", "{\"code\": 400,\"status\": \"" + status + "\",\"path\": \"/api/set/scanner\"}");
    });

    server.on("/api/get/scanner", HTTP_GET, [](AsyncWebServerRequest *request) {
        JsonDocument doc;
        doc["code"] = 200;
        doc["status"] = "ok";
        doc["path"] = "/api/get/scanner";
        JsonObject data = doc.createNestedObject("data");
        data["z_axis_counter"] = get_z_axis_counter();
        request->send(200, "application/json", doc.as<String>());
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