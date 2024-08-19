// Path: src/components/module.cpp
#include "components/module.h"    // include/components/module.h

const char* MODULE_TAG = MODULE_TAG_NAME;

Adafruit_VL53L1X vl53 = Adafruit_VL53L1X();

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

uint8_t _command = 0;
uint64_t _steps = 0;

uint32_t z_steps = 0;
uint32_t x_y_steps = 0;

bool vl53_ready = false;
bool sd_card_ready = false;

String project_name = "";
String message = "";

uint64_t msg_len = 0;

unsigned long start_time = 0;
unsigned long last_send_data_time = 0;

void writeFile(fs::FS &fs, const char * path, const char * message);
void appendFile(fs::FS &fs, const char * path, const char * message);

uint16_t findMode(const std::vector<uint16_t>& numbers) {
    std::unordered_map<uint16_t, uint16_t> frequencyMap;

    for (uint16_t num : numbers) {
        frequencyMap[num]++;
    }

    uint16_t mode = numbers[0];
    uint16_t maxCount = 0;

    for (const auto& pair : frequencyMap) {
        if (pair.second > maxCount) {
            maxCount = pair.second;
            mode = pair.first;
        }
    }

    return mode;
}

void module_data_init() {
    get_module( &z_axis_max, &z_axis_start_step, &z_axis_delay_time, &z_axis_one_time_step, 
                &x_y_axis_max, &x_y_axis_check_times, &x_y_axis_step_delay_time, &x_y_axis_one_time_step,
                &vl53l1x_center, &vl53l1x_timeing_budget);
    ESP_LOGD(MODULE_TAG, "Z axis max: %u, start step: %u, delay time: %u, one time step: %u", z_axis_max, z_axis_start_step, z_axis_delay_time, z_axis_one_time_step);
    ESP_LOGD(MODULE_TAG, "X Y axis max: %u, check times: %u, step delay time: %u, one time step: %u", x_y_axis_max, x_y_axis_check_times, x_y_axis_step_delay_time, x_y_axis_one_time_step);
    ESP_LOGD(MODULE_TAG, "VL53L1X center: %u, timing budget: %u", vl53l1x_center, vl53l1x_timeing_budget);
}

void motor_init() {
    pinMode(Z_AXIS_MOTOR_STEP, OUTPUT);
    pinMode(Z_AXIS_MOTOR_DIR, OUTPUT);
    pinMode(Z_AXIS_MOTOR_EN, OUTPUT);

    pinMode(X_Y_AXIS_MOTOR_STEP, OUTPUT);
    pinMode(X_Y_AXIS_MOTOR_DIR, OUTPUT);
    pinMode(X_Y_AXIS_MOTOR_EN, OUTPUT);

    digitalWrite(Z_AXIS_MOTOR_EN, LOW);
    digitalWrite(X_Y_AXIS_MOTOR_EN, LOW);

    digitalWrite(X_Y_AXIS_MOTOR_DIR, LOW);
}

void sd_card_init() {
    SPI.begin(SPI_SCK, SPI_MISO, SPI_MOSI, SPI_CS);
    SPI.setDataMode(SPI_MODE0);

    if (!SD.begin(SPI_CS, SPI)) {
        ESP_LOGE(MODULE_TAG, "Failed to mount SD card");
        return;
    }

    if(SD.cardType() == CARD_NONE){
        ESP_LOGE(MODULE_TAG, "No SD card attached");
        return;
    }

    ESP_LOGD(MODULE_TAG, "SD Card Type: %s", SD.cardType() == CARD_MMC ? "MMC" : SD.cardType() == CARD_SD ? "SDSC" : "SDHC");
    sd_card_ready = true;
}

void vl53_init() {
    Wire.begin();

    if (!vl53.begin(0x29, &Wire)) {
        ESP_LOGE(MODULE_TAG, "Failed to boot VL53L1X: ", vl53.vl_status);
        vl53.end();
        return;
    }

    // vl53.VL53L1X_SetDistanceThreshold(40, 142, 2, 1);

    if (!vl53.startRanging()) {
        ESP_LOGE(MODULE_TAG, "Failed to start VL53L1X ranging: ", vl53.vl_status);
        vl53.end();
        return;
    }

    vl53_ready = true;

    vl53.setTimingBudget(vl53l1x_timeing_budget);
    ESP_LOGD(MODULE_TAG, "VL53L1X Timing budget (ms): %u", vl53.getTimingBudget());
}

void module_init() {
    pinMode(LED_PIN, OUTPUT);
    pinMode(BUTTON_PIN, INPUT_PULLDOWN);

    module_data_init();
    motor_init();
    sd_card_init();

    vl53_init();
}

void set_project_name(const char* name) {
    project_name = name;
    writeFile(SD, ("/" + project_name + ".csv").c_str(), "");
}

void set_command(uint8_t command, uint32_t steps) {
    ESP_LOGI(MODULE_TAG, "Set command: %u, step: %u", command, steps);
    _command = command;
    _steps = steps;

    if (_command == SCANNER_COMMAND_START) {
        start_time = millis();
    }

    msg_len = 0;
    x_y_steps = 0;
}

uint32_t get_z_axis_counter() {
    return z_steps;
}

void z_axis_motor_step(bool direction) {
    if ( ( z_steps >= z_axis_max && direction == Z_AXIS_MOTOR_UP ) || 
         ( ( digitalRead(BUTTON_PIN) == LOW || z_steps <= 0 ) && direction == Z_AXIS_MOTOR_DOWN ) ) {

        ESP_LOGD(MODULE_TAG, "Z_AXIS_MOTOR_FULL_STEP: %u", z_steps);

        return;
    }

    digitalWrite(Z_AXIS_MOTOR_DIR, direction);

    digitalWrite(Z_AXIS_MOTOR_STEP, HIGH);
    delayMicroseconds(z_axis_delay_time);
    digitalWrite(Z_AXIS_MOTOR_STEP, LOW);
    delayMicroseconds(z_axis_delay_time);

    if (direction == Z_AXIS_MOTOR_UP) {
        z_steps++;
    } else {
        z_steps--;
    }
}

void x_y_axis_motor_step(bool direction) {
    digitalWrite(X_Y_AXIS_MOTOR_DIR, direction);

    digitalWrite(X_Y_AXIS_MOTOR_STEP, HIGH);
    delayMicroseconds(x_y_axis_step_delay_time);
    digitalWrite(X_Y_AXIS_MOTOR_STEP, LOW);
    delayMicroseconds(x_y_axis_step_delay_time);
}

void scanner_loop() {
    if (_command == SCANNER_COMMAND_STOP) {
        if (millis() - last_send_data_time > SEND_DATA_TIME_MS) {
            last_send_data_time = millis();
            String send_msg = "{\"z_steps\":\"" + String(z_steps);
            while (vl53_ready) {
                if (vl53.dataReady()) {
                    uint32_t distance;
                    uint8_t status;
                    vl53.startRanging();
                    vl53.VL53L1X_GetRangeStatus(&status);
                    if(vl53.GetDistance(&distance) == 0) {
                        send_msg += "\",\"vl53l1x\":" + String(distance);
                    } else {
                        send_msg += "\",\"vl53l1x\":-1";
                    }
                    Serial.printf("status: %u\n", status);
                    break;
                }
            }
            send_msg += "}";
            ws_send_text(send_msg.c_str());
            message = "";
            Serial.println(send_msg);
        }
        delay(100);
    } else if (_command == SCANNER_COMMAND_HOME) {
        ESP_LOGD(MODULE_TAG, "Home command");
        z_steps = z_axis_max;
        while( digitalRead(BUTTON_PIN) == HIGH ) {
            z_axis_motor_step(Z_AXIS_MOTOR_DOWN);
        }
        z_steps = 0;
        set_command(SCANNER_COMMAND_STOP);
        ESP_LOGD(MODULE_TAG, "Home command done");
    } else if (_command == SCANNER_COMMAND_START && project_name.length() > 0) {
        for (uint16_t i = 0; i < x_y_axis_one_time_step; i++) {
            x_y_axis_motor_step(HIGH);
        }
        if(vl53_ready) {
            std::vector<uint16_t> distances;
            for (uint16_t i = 0; i < x_y_axis_check_times; i++) {
                while(!vl53.dataReady()) {
                    delay(10);
                }
                distances.push_back(vl53.distance());
                vl53.clearInterrupt();
            }
            uint16_t distance = findMode(distances);

            float angle = x_y_steps * MOTOR1_DEFAULT_MICRO_STEP_DEGREE; // 1 / 32 steps, 1.8 degree per step
            double r = double(vl53l1x_center) - double(distance);
            double x = r * cos(angle * PI / 180);
            double y = r * sin(angle * PI / 180);
            ESP_LOGI(MODULE_TAG, "Time: %.4f, angle: %.5f, x_y_steps: %u, z_steps: %u", ((millis() - start_time) / 1000.0), angle, x_y_steps, z_steps);
            ESP_LOGI(MODULE_TAG, "R: %.2f, X: %.5f, Y: %.5f, Z: %.5f", r, x, y, z_steps * Z_AXIS_STEP_MM);
            Serial.printf("%.5f, %.5f, %.5f\n", x, y, z_steps * Z_AXIS_STEP_MM);
            message += "[" + String(x * 0.01) + "," + String(y * 0.01) + "," + String(z_steps * Z_AXIS_STEP_FLOAT) + "]";
            if (( msg_len++ % 10 ) == 0) {
                if (sd_card_ready) {
                    // appendFile(SD, ("/" + project_name + ".csv").c_str() , message.c_str());
                }
                String send_msg = "{\"name\":\"" + project_name + "\",\"points_count\":" + String(msg_len) + ",\"is_last\":false,\"points\":[" + message + "]}";
                ws_send_text(send_msg.c_str());
                message = "";
            } else {
                message += ",";
            }

        }

        x_y_steps+=x_y_axis_one_time_step;
        if(x_y_steps >= x_y_axis_max) {
            ESP_LOGD(MODULE_TAG, "X Y Full step max count, Z axis steps: %u", z_steps);
            for (uint16_t i = 0; i < z_axis_one_time_step; i++) {
                z_axis_motor_step(Z_AXIS_MOTOR_UP);
            }
            x_y_steps = 0;
        }

        if (z_steps >= z_axis_max) {
            ESP_LOGD(MODULE_TAG, "Z Full step max count and Finish");
            project_name = "";
            set_command(SCANNER_COMMAND_STOP);
        }

    } else {
        if (_command == SCANNER_COMMAND_UP) {
            z_axis_motor_step(Z_AXIS_MOTOR_UP);
        } else if (_command == SCANNER_COMMAND_DOWN) {
            z_axis_motor_step(Z_AXIS_MOTOR_DOWN);
        } else if (_command == SCANNER_COMMAND_RIGHT) {
            x_y_axis_motor_step(HIGH);
        } else if (_command == SCANNER_COMMAND_LEFT) {
            x_y_axis_motor_step(LOW);
        }

        _steps--;
        if (_steps <= 0) {
            ESP_LOGD(MODULE_TAG, "Command done");
            set_command(SCANNER_COMMAND_STOP);
        }
    }
}

void writeFile(fs::FS &fs, const char * path, const char * message) {
    Serial.printf("Writing file: %s\n", path);

    File file = fs.open(path, FILE_WRITE);
    if(!file){
        Serial.println("Failed to open file for writing");
        return;
    }
    if(file.print(message)){
        Serial.println("File written");
    } else {
        Serial.println("Write failed");
    }
    file.close();
}

void appendFile(fs::FS &fs, const char * path, const char * message) {
    Serial.printf("Appending to file: %s\n", path);

    File file = fs.open(path, FILE_APPEND);
    if(!file){
        Serial.println("Failed to open file for appending");
        return;
    }
    if(file.print(message)){
        Serial.println("Message appended");
    } else {
        Serial.println("Append failed");
    }
    file.close();
}