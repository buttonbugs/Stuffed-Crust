#include <Arduino.h>
#include <Servo.h>
#include "melty_config.h"
#include "motor_driver.h"

Servo motor1;
Servo motor2;

constexpr uint16_t ESC_MIN_US  = 1000;
constexpr uint16_t ESC_MAX_US  = 2000;
constexpr uint16_t ESC_STOP_US = 1000;

// Map 0-100% to 1000-2000us
static uint16_t toMicroseconds(float throttle_percent) {
    throttle_percent = constrain(throttle_percent, 0.0f, 1.0f);
    return ESC_MIN_US + static_cast<uint16_t>(throttle_percent * (ESC_MAX_US - ESC_MIN_US));
}

void motor_on(float throttle_percent, Servo &motor) {
    motor.writeMicroseconds(toMicroseconds(throttle_percent));
}

void motor_1_on(float throttle_percent) {
    motor_on(throttle_percent, motor1);
}

void motor_2_on(float throttle_percent) {
    motor_on(throttle_percent, motor2);
}

void motor_coast(Servo &motor) {
    // In normal mode, coast is a low throttle rather than zero
    // to keep the ESC signal alive — adjust this value to suit your motor
    motor.writeMicroseconds(1100);
}

void motor_1_coast() {
    motor_coast(motor1);
}

void motor_2_coast() {
    motor_coast(motor2);
}

void motor_off(Servo &motor) {
    motor.writeMicroseconds(ESC_STOP_US);
}

void motor_1_off() {
    motor_off(motor1);
}

void motor_2_off() {
    motor_off(motor2);
}

void motors_off() {
    motor_1_off();
    motor_2_off();
}

void init_motors() {
    motor1.attach(MOTOR_PIN1, ESC_MIN_US, ESC_MAX_US);
    motor2.attach(MOTOR_PIN2, ESC_MIN_US, ESC_MAX_US);

    // Send stop signal immediately then hold to arm
    motors_off();
    delay(5000);
}