// Path: include/components/module.h
#ifndef __3D_SCANNER_MODULE_H__
#define __3D_SCANNER_MODULE_H__

#include <Arduino.h>

#include <SD.h>
#include <FS.h>
#include <SPI.h>
#include <Wire.h>

#include <math.h>

#include "esp_log.h"

#include <ArduinoJson.h>
#include <Adafruit_VL53L1X.h>

#include "components/data.h"
#include "components/server.h"

#define MODULE_TAG_NAME "module"

#define SPI_MOSI 23
#define SPI_MISO 19
#define SPI_SCK  18
#define SPI_CS    5

// Z axis
#define MOTOR1_STEP 25
#define MOTOR1_DIR  26
#define MOTOR1_EN   32

#define MOTOR1_UP   HIGH
#define MOTOR1_DOWN LOW

#define MOTOR1_DEFAULT_STEP_DEGREE 1.8
#define MOTOR1_DEFAULT_MICRO_STEP  32
#define MOTOR1_DEFAULT_MICRO_STEP_DEGREE ((MOTOR1_DEFAULT_STEP_DEGREE) / (MOTOR1_DEFAULT_MICRO_STEP))

#define Z_AXIS_MOTOR_STEP (MOTOR1_STEP)
#define Z_AXIS_MOTOR_DIR  (MOTOR1_DIR)
#define Z_AXIS_MOTOR_EN   (MOTOR1_EN)

#define Z_AXIS_MOTOR_UP   (MOTOR1_UP)
#define Z_AXIS_MOTOR_DOWN (MOTOR1_DOWN)

#define Z_AXIS_STEP_MM 0.00125
#define Z_AXIS_STEP_FLOAT ((Z_AXIS_STEP_MM) * 0.01)

// X and Y axis
#define MOTOR2_STEP 27
#define MOTOR2_DIR  13
#define MOTOR2_EN   33

#define X_Y_AXIS_MOTOR_STEP (MOTOR2_STEP)
#define X_Y_AXIS_MOTOR_DIR  (MOTOR2_DIR)
#define X_Y_AXIS_MOTOR_EN   (MOTOR2_EN)

// Stop Button
#define BUTTON_PIN 35

#define LED_PIN 2

#define SCANNER_COMMAND_STOP  0
#define SCANNER_COMMAND_HOME  1
#define SCANNER_COMMAND_NEW   2
#define SCANNER_COMMAND_START 3
#define SCANNER_COMMAND_UP    4
#define SCANNER_COMMAND_DOWN  5
#define SCANNER_COMMAND_RIGHT 6
#define SCANNER_COMMAND_LEFT  7

#define SEND_DATA_TIME_MS 1000

void set_project_name(const char* name);
void module_init();
void set_command(uint8_t command, uint32_t steps = 0);
void scanner_loop();

uint32_t get_z_axis_counter();

#endif
