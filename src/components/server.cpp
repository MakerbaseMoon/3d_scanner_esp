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
    DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");

    WEBSITE

    server.on("/api/info", HTTP_GET, [](AsyncWebServerRequest *request) {
        char* ssid = NULL;
        char* password = NULL;
        char* ap_ssid = NULL;
        char* ap_password = NULL;
        char* hostname = NULL;
        char* username = NULL;
        char* repo = NULL;

        uint16_t z_axis_max = NVS_Z_AXIS_MAX_DEFAULT;
        uint16_t z_axis_start_step = NVS_Z_AXIS_START_STEP_DEFAULT;
        uint16_t z_axis_delay_time = NVS_Z_AXIS_DELAY_TIME_DEFAULT;
        uint16_t z_axis_one_time_step = NVS_Z_AXIS_ONE_TIME_STEP_DEFAULT;

        uint16_t x_y_axis_max = NVS_X_Y_AXIS_MAX_DEFAULT;
        uint16_t x_y_axis_check_times = NVS_X_Y_AXIS_CHECK_TIMES_DEFAULT;
        uint16_t x_y_axis_step_delay_time = NVS_X_Y_AXIS_STEP_DELAY_TIME_DEFAULT;
        uint16_t x_y_axis_one_time_step = NVS_X_Y_AXIS_ONE_TIME_STEP_DEFAULT;

        uint16_t vl53l1x_center = NVS_VL53L1X_CENTER_DEFAULT;
        uint16_t vl53l1x_timeing_budget = NVS_VL53L1X_TIMEING_BUDGET_DEFAULT;

        get_sta_wifi(&ssid, &password);
        get_ap_wifi(&ap_ssid, &ap_password);
        get_mdns_hostname(&hostname);
        get_module(&z_axis_max, &z_axis_start_step, &z_axis_delay_time, &z_axis_one_time_step, 
                    &x_y_axis_max, &x_y_axis_check_times, &x_y_axis_step_delay_time, &x_y_axis_one_time_step,
                    &vl53l1x_center, &vl53l1x_timeing_budget);
        get_github(&username, &repo);
        if (ssid != NULL && password != NULL && ap_ssid != NULL && ap_password != NULL && hostname != NULL && username != NULL && repo != NULL) {
            JsonDocument doc;
            doc["code"] = 200;
            doc["status"] = "ok";
            doc["path"] = "/api/info";
            JsonObject data = doc.createNestedObject("data");
            data["version"] = ESP32_3D_SCANNER_VERSION;
            data["mdns"] = hostname;
            JsonObject sta = data.createNestedObject("sta");
            sta["ssid"] = ssid;
            sta["password"] = password;
            JsonObject ap = data.createNestedObject("ap");
            ap["ssid"] = ap_ssid;
            ap["password"] = ap_password;
            JsonObject github = data.createNestedObject("github");
            github["username"] = username;
            github["repo"] = repo;
            JsonObject module = data.createNestedObject("module");

            module["z_axis_max"] = z_axis_max;
            module["z_axis_start_step"] = z_axis_start_step;
            module["z_axis_delay_time"] = z_axis_delay_time;
            module["z_axis_one_time_step"] = z_axis_one_time_step;
            module["x_y_axis_max"] = x_y_axis_max;
            module["x_y_axis_check_times"] = x_y_axis_check_times;
            module["x_y_axis_step_delay_time"] = x_y_axis_step_delay_time;
            module["x_y_axis_one_time_step"] = x_y_axis_one_time_step;
            module["vl53l1x_center"] = vl53l1x_center;
            module["vl53l1x_timeing_budget"] = vl53l1x_timeing_budget;
            
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

    server.on("/api/set/data", HTTP_GET, [](AsyncWebServerRequest *request) {
        try {
            if (request->getParam("sta_ssid") != NULL && request->getParam("sta_password") != NULL) {
                set_sta_wifi(request->getParam("sta_ssid")->value().c_str(), request->getParam("sta_password")->value().c_str());
            }

            if (request->getParam("ap_ssid") != NULL && request->getParam("ap_password") != NULL) {
                set_ap_wifi(request->getParam("ap_ssid")->value().c_str(), request->getParam("ap_password")->value().c_str());
            }

            if (request->getParam("mdns") != NULL) {
                set_mdns_hostname(request->getParam("mdns")->value().c_str());
            }

            if (request->getParam("github_username") != NULL && request->getParam("github_repo") != NULL) {
                set_github(request->getParam("github_username")->value().c_str(), request->getParam("github_repo")->value().c_str());
            }

            if (request->getParam("z_axis_max") != NULL && request->getParam("z_axis_start_step") != NULL && request->getParam("z_axis_delay_time") != NULL && request->getParam("z_axis_one_time_step") != NULL &&
                request->getParam("x_y_axis_max") != NULL && request->getParam("x_y_axis_check_times")  && request->getParam("x_y_axis_step_delay_time") != NULL && request->getParam("x_y_axis_one_time_step") != NULL &&
                request->getParam("vl53l1x_center") != NULL && request->getParam("vl53l1x_timeing_budget") != NULL) {
                set_module(request->getParam("z_axis_max")->value().toInt(), request->getParam("z_axis_start_step")->value().toInt(), request->getParam("z_axis_delay_time")->value().toInt(), request->getParam("z_axis_one_time_step")->value().toInt(),
                            request->getParam("x_y_axis_max")->value().toInt(), request->getParam("x_y_axis_check_times")->value().toInt(), request->getParam("x_y_axis_step_delay_time")->value().toInt(), request->getParam("x_y_axis_one_time_step")->value().toInt(),
                            request->getParam("vl53l1x_center")->value().toInt(), request->getParam("vl53l1x_timeing_budget")->value().toInt());
            }

            request->send(200, "application/json", "{\"code\": 200,\"status\": \"ok\",\"path\": \"/api/set/data\"}");

        } catch(const std::exception& e) {
            ESP_LOGE(SERVER_TAG, "Error: %s", e.what());
            request->send(500, "application/json", "{\"code\": 500,\"status\": \"Server Error\",\"path\": \"/api/set/data\"}");
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
                }  else if(command == "end") {
                    ESP_LOGD(SERVER_TAG, "END command");
                    set_command(SCANNER_COMMAND_STOP);
                    set_project_name("");
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

        request->send(code, "application/json", "{\"code\":" + String(code) + ",\"status\": \"" + status + "\",\"path\": \"/api/set/scanner\"}");
    });

    server.on("/api/ota", HTTP_GET, [](AsyncWebServerRequest *request) {
        try{
            if (request->getParam("username") != NULL && request->getParam("repo") != NULL && request->getParam("id") != NULL) {
                flash_firmware(request->getParam("username")->value().c_str(), request->getParam("repo")->value().c_str(), request->getParam("asset_id")->value().toInt());
                request->send(200, "application/json", "{\"code\": 200,\"status\": \"ok\",\"path\": \"/api/ota\"}");
            } else {
                request->send(400, "application/json", "{\"code\": 400,\"status\": \"param not found\",\"path\": \"/api/ota\"}");
            }
        }
        catch(const std::exception& e) {
            ESP_LOGE(SERVER_TAG, "Error: %s", e.what());
            request->send(500, "application/json", "{\"code\": 500,\"status\": \"Server Error\",\"path\": \"/api/ota\"}");
        }
    });

    server.on("/api/restart", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(200, "application/json", "{\"code\": 200,\"status\": \"ok\",\"path\": \"/api/restart\"}");
        delay(1000);
        ESP.restart();
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