// Path: src/components/module.cpp
#include "components/module.h"    // include/components/module.h

const char* MODULE_TAG = MODULE_TAG_NAME;

#ifdef CONFIG_VL53L1X
Adafruit_VL53L1X vl53 = Adafruit_VL53L1X();
#else
Adafruit_VL53L0X vl53 = Adafruit_VL53L0X();
#endif

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

uint8_t _command = SCANNER_COMMAND_STOP;
uint64_t _steps = 0;

uint32_t z_steps = 0;
uint32_t x_y_steps = 0;

bool vl53_ready = false;
bool sd_card_ready = false;

String project_name = "";
String message = "";

uint64_t point_count = 0;

uint16_t distance_max = 0;
uint16_t distance_min = 0;

unsigned long start_time = 0;
unsigned long last_send_data_time = 0;

void writeFile(fs::FS &fs, const char * path, const char * message);
void appendFile(fs::FS &fs, const char * path, const char * message);
uint16_t get_count_distance(uint16_t count);
uint16_t get_distance();
void get_x_y(double angle, double r, double* x, double* y);

int16_t findMode(const std::vector<int16_t>& numbers) {
    std::unordered_map<int16_t, int16_t> frequencyMap;

    for (int16_t num : numbers) {
        frequencyMap[num]++;
    }

    int16_t mode = numbers[0];
    int16_t maxCount = 0;

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
    distance_max = vl53l1x_center + 70;
    distance_min = vl53l1x_center - 70;
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

void vl53_init() {
    if(!Wire.begin()) {
        return;
    }

#ifdef CONFIG_VL53L1X
    if (!vl53.begin(0x29, &Wire)) {
        ESP_LOGE(MODULE_TAG, "Failed to boot VL53L1X: ", vl53.vl_status);
        vl53.end();
        return;
    }

    uint32_t distance;
    uint8_t status;
    vl53.VL53L1X_GetRangeStatus(&status);

    if(vl53.GetDistance(&distance) != 0) {
        vl53.clearInterrupt();
        ESP_LOGE(MODULE_TAG, "Failed to get VL53L1X distance: ", vl53.vl_status);
        vl53.stopRanging();
        vl53.end();
        return;
    }

    if (!vl53.startRanging()) {
        ESP_LOGE(MODULE_TAG, "Failed to start VL53L1X ranging: ", vl53.vl_status);
        vl53.stopRanging();
        vl53.end();
        return;
    }

    vl53.setTimingBudget(vl53l1x_timeing_budget);
    ESP_LOGD(MODULE_TAG, "VL53L1X Timing budget (ms): %u", vl53.getTimingBudget());
#else
    if (!vl53.begin()) {
        ESP_LOGE(MODULE_TAG, "Failed to boot VL53L0X");
        return;
    }

    vl53.setMeasurementTimingBudgetMicroSeconds(vl53l1x_timeing_budget * 1000);
    if (!vl53.startRangeContinuous()) {
        ESP_LOGE(MODULE_TAG, "Failed to start VL53L0X ranging");
        vl53.stopRangeContinuous();
        return;
    }
#endif
    vl53_ready = true;

}

void module_init() {
    pinMode(LED_PIN, OUTPUT);
    pinMode(BUTTON_PIN, INPUT_PULLDOWN);

    module_data_init();
    motor_init();

    vl53_init();
}

void set_project_name(const char* name) {
    project_name = name;
    point_count = 0;
}

void set_command(uint8_t command, uint32_t steps) {
    ESP_LOGI(MODULE_TAG, "Set command: %u, step: %u", command, steps);
    _command = command;
    _steps = steps;

    if (_command == SCANNER_COMMAND_START) {
        start_time = millis();
    }

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
            String send_msg =   "{\"z_steps\":" + String(z_steps) + 
                                ",\"vl53l1x\":" + String(get_distance()) + 
                                ",\"name\":\"" + project_name + "\",\"status\":\"stop\"}";
            ws_send_text(send_msg.c_str());
        }
        delay(800);
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

        double x = 0, y = 0, r = 20;
        if(vl53_ready) {
            int16_t distanceMode = get_count_distance(x_y_axis_check_times);
            r = fabs(double(vl53l1x_center) - double(distanceMode));
            get_x_y(x_y_steps * MOTOR1_DEFAULT_MICRO_STEP_DEGREE, r,  &x,  &y);
        } else {
            get_x_y(x_y_steps * MOTOR1_DEFAULT_MICRO_STEP_DEGREE, r,  &x,  &y);
            delay(800);
        }

        String point = "[" + String(x) + "," + String(y) + "," + String(z_steps * 0.00125) + "]";
        message = "{\"name\":\"" + project_name + "\"" +
                    ",\"status\":\"scan\"" +
                    ",\"points_count\":" + String(++point_count) +
                    ",\"time\":" + String((millis() - start_time) / 1000.0) +
                    ",\"is_last\":false" +
                    ",\"z_steps\":" + String(z_steps) +
                    ",\"r\":" + String(r) +
                    ",\"points\":[" + point + "]}";

        ws_send_text(message.c_str());

        x_y_steps += x_y_axis_one_time_step;
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

void get_x_y(double angle, double r, double* x, double* y) {
    *x = r * cos(angle * PI / 180);
    *y = r * sin(angle * PI / 180);
}

uint16_t get_count_distance(uint16_t count) {
    if (!vl53_ready) return 0;
    std::vector<int16_t> distances;
    while(count > 0) {
        #ifdef CONFIG_VL53L1X
        while(!vl53.dataReady()) { delay(20); }
        #endif
        delay(20);
        uint16_t distance = get_distance();
        if (distance < distance_min || distance > distance_max) continue; 
        distances.push_back(uint16_t(distance));
        count--;
    }

    if (distances.size() == 0) return 0;
    return findMode(distances);
} 

uint16_t get_distance() {
    if (!vl53_ready) return 0;
    while(1) {
        #ifdef CONFIG_VL53L1X
        if (vl53.dataReady()) {
            uint32_t distance;
            uint8_t status;
            vl53.VL53L1X_GetRangeStatus(&status);
            if(vl53.GetDistance(&distance) == 0) {
                vl53.clearInterrupt();
                return distance;
            } else {
                vl53.clearInterrupt();
                return 0;
            }
        }
        #else
        if (vl53.isRangeComplete()) {
            return vl53.readRange();
        }
        #endif
    }
}