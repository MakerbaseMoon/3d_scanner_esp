// Path: src/components/module.cpp
#include "components/module.h"    // include/components/module.h

const char* MODULE_TAG = MODULE_TAG_NAME;

Adafruit_VL53L1X vl53 = Adafruit_VL53L1X();

uint8_t _command = 0;
uint64_t _steps = 0;

uint32_t z_steps = 0;
uint32_t x_y_steps = 0;

bool vl53_ready = false;
bool sd_card_ready = false;

String project_name = "";
String message = "";

void writeFile(fs::FS &fs, const char * path, const char * message);
void appendFile(fs::FS &fs, const char * path, const char * message);

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
    SPI.begin();

    if (!SD.begin()) {
        ESP_LOGE(MODULE_TAG, "Failed to mount SD card");
        return;
    }

    if(SD.cardType() == CARD_NONE){
        ESP_LOGE(MODULE_TAG, "No SD card attached");
        return;
    }

    sd_card_ready = true;
}

void vl53_init() {
    Wire.begin();

    if (!vl53.begin()) {
        ESP_LOGE(MODULE_TAG, "Failed to boot VL53L1X: ", vl53.vl_status);
        vl53.end();
        return;
    } 

    if (!vl53.startRanging()) {
        ESP_LOGE(MODULE_TAG, "Failed to start VL53L1X ranging: ", vl53.vl_status);
        vl53.end();
        return;
    }

    vl53_ready = true;

    vl53.setTimingBudget(50);
    ESP_LOGD(MODULE_TAG, "VL53L1X Timing budget (ms): %u", vl53.getTimingBudget());
}

void module_init() {
    motor_init();
    vl53_init();
    sd_card_init();

    pinMode(LED_PIN, OUTPUT);
    pinMode(BUTTON_PIN, INPUT_PULLDOWN);
}

void set_project_name(const char* name) {
    project_name = name;
    writeFile(SD, ("/" + project_name + ".csv").c_str(), "");
}

void set_command(uint8_t command, uint32_t steps) {
    ESP_LOGI(MODULE_TAG, "Set command: %u, step: %u", command, steps);
    _command = command;
    _steps = steps;

    x_y_steps = 0;
}

uint32_t get_z_axis_counter() {
    return z_steps;
}

void z_axis_motor_step(bool direction) {
    if ( ( z_steps >= Z_AXIS_MAX_STEPS && direction == Z_AXIS_MOTOR_UP ) || 
         ( ( digitalRead(BUTTON_PIN) == LOW || z_steps <= 0 ) && direction == Z_AXIS_MOTOR_DOWN ) ) {

        ESP_LOGD(MODULE_TAG, "Z_AXIS_MOTOR_FULL_STEP: %u", z_steps);

        return;
    }

    digitalWrite(Z_AXIS_MOTOR_DIR, direction);

    digitalWrite(Z_AXIS_MOTOR_STEP, HIGH);
    delayMicroseconds(Z_AXIS_STEP_DELAY);
    digitalWrite(Z_AXIS_MOTOR_STEP, LOW);
    delayMicroseconds(Z_AXIS_STEP_DELAY);

    if (direction == Z_AXIS_MOTOR_UP) {
        z_steps++;
    } else {
        z_steps--;
    }
}

void x_y_axis_motor_step(bool direction) {
    digitalWrite(X_Y_AXIS_MOTOR_DIR, direction);

    digitalWrite(X_Y_AXIS_MOTOR_STEP, HIGH);
    delayMicroseconds(X_Y_AXIS_STEP_DELAY);
    digitalWrite(X_Y_AXIS_MOTOR_STEP, LOW);
    delayMicroseconds(X_Y_AXIS_STEP_DELAY);
}

void scanner_loop() {
    if (_command == SCANNER_COMMAND_STOP) {

        delay(100);
    } else if (_command == SCANNER_COMMAND_HOME) {
        ESP_LOGD(MODULE_TAG, "Home command");
        z_steps = Z_AXIS_MAX_STEPS;
        while( digitalRead(BUTTON_PIN) == HIGH ) {
            z_axis_motor_step(Z_AXIS_MOTOR_DOWN);
        }
        z_steps = 0;
        set_command(SCANNER_COMMAND_STOP);
        ESP_LOGD(MODULE_TAG, "Home command done");
    } else if (_command == SCANNER_COMMAND_START && project_name.length() > 0) {
        x_y_axis_motor_step(HIGH);
        while(vl53_ready) {
            if( vl53.dataReady()) {
                float angle = x_y_steps * 0.05625; // 1 / 32 steps, 1.8 degree per step
                double r = vl53.distance() - 110;
                vl53.clearInterrupt();
                double x = r * cos(angle * PI / 180);
                double y = r * sin(angle * PI / 180);
                ESP_LOGI(MODULE_TAG, "VL53L1X Distance: %u, angle: %.5f, x_y_steps: %u, z_steps: %u", r, angle, x_y_steps, z_steps);
                ESP_LOGI(MODULE_TAG, "X: %.5f, Y: %.5f, Z: %.5f", x, y, z_steps / 1000.0);
                message = String(x) + "," + String(y) + "," + String(z_steps / 1000.0) + "\n";
                if (( x_y_steps % 100 ) == 0) {
                    if (sd_card_ready) {
                        // appendFile(SD, ("/" + project_name + ".csv").c_str() , message.c_str());
                    }

                    message = "";
                }
                break;
            }
        }

        x_y_steps++;
        if(x_y_steps >= FULL_STEP_MAX_COUNT) {
            ESP_LOGD(MODULE_TAG, "X Y Full step max count, Z axis steps: %u", z_steps);
            for (uint32_t i = 0; i < 100; i++) {
                z_axis_motor_step(Z_AXIS_MOTOR_UP);
            }
            x_y_steps = 0;
        }

        if (z_steps >= Z_AXIS_MAX_STEPS) {
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