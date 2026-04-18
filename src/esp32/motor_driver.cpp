// ESP32 Motor Driver - PWM based control for brushless ESCs
// Uses the ESP32's LEDC (LED Control) peripheral for PWM generation

#include <Arduino.h>
#include "melty_config.h"
#include "motor_driver.h"
#include "map_float.h"

// PWM Configuration for ESP32
#define PWM_FREQUENCY 50        // 50Hz for ESC compatibility (standard servo/ESC frequency)
#define PWM_RESOLUTION 16       // 16-bit resolution (0-65535)
#define PWM_MAX ((1 << PWM_RESOLUTION) - 1)

// LEDC channels
#define MOTOR1_CHANNEL 0
#define MOTOR2_CHANNEL 1

// Convert microseconds to PWM duty cycle for 50Hz
// 1000us = 0%, 1500us = 50%, 2000us = 100%
static uint32_t microsecondsToLEDC(uint16_t microseconds) {
    // At 50Hz, period = 20000us
    // duty_cycle = (microseconds / 20000) * 65535
    return map(microseconds, 1000, 2000, 0, PWM_MAX);
}

// ESC PWM ranges
constexpr uint16_t ESC_MIN_US   = 1000;
constexpr uint16_t ESC_MAX_US   = 2000;
constexpr uint16_t ESC_STOP_US  = 1500;
constexpr uint16_t ESC_COAST_DIFF_US = 150;

// Map throttle (-1.0 to 1.0) to microseconds
static uint16_t throttleToMicroseconds(float throttle_percent) {
    throttle_percent = constrain(throttle_percent, -1.0f, 1.0f);
    return map_float(throttle_percent, -1.0f, 1.0f, ESC_MIN_US, ESC_MAX_US);
}

static void setPWM(uint8_t channel, uint16_t microSeconds) {
    uint32_t duty = microsecondsToLEDC(microSeconds);
    ledcWrite(channel, duty);
}

void motor_on(float throttle_percent, uint8_t channel) {
    uint16_t us = throttleToMicroseconds(throttle_percent);
    setPWM(channel, us);
    
    if (Serial) {
        Serial.print("Motor ");
        Serial.print(channel);
        Serial.print(" PWM: ");
        Serial.println(us);
    }
}

void motor_1_on(float throttle_percent) {
    motor_on(throttle_percent, MOTOR1_CHANNEL);
}

void motor_2_on(float throttle_percent) {
    motor_on(throttle_percent, MOTOR2_CHANNEL);
}

void motor_coast(uint8_t channel) {
    // Coast maintains a weak signal to keep ESC alive
    setPWM(channel, ESC_STOP_US + ESC_COAST_DIFF_US);
}

void motor_1_coast() {
    motor_coast(MOTOR1_CHANNEL);
}

void motor_2_coast() {
    motor_coast(MOTOR2_CHANNEL);
}

void motor_off(uint8_t channel) {
    setPWM(channel, ESC_STOP_US);
}

void motor_1_off() {
    motor_off(MOTOR1_CHANNEL);
}

void motor_2_off() {
    motor_off(MOTOR2_CHANNEL);
}

void motors_off() {
    motor_1_off();
    motor_2_off();
}

void init_motors() {
    // Configure LEDC for Motor 1
    ledcSetup(MOTOR1_CHANNEL, PWM_FREQUENCY, PWM_RESOLUTION);
    ledcAttachPin(MOTOR_PIN1, MOTOR1_CHANNEL);
    
    // Configure LEDC for Motor 2
    ledcSetup(MOTOR2_CHANNEL, PWM_FREQUENCY, PWM_RESOLUTION);
    ledcAttachPin(MOTOR_PIN2, MOTOR2_CHANNEL);
    
    // Send stop signal and arm ESCs
    motors_off();
    delay(250);
    
    Serial.println("Motors initialized (ESP32 PWM)");
}
