#include <Arduino.h>
#include "melty_config.h"
#include "motor_driver.h"
#include "map_float.h"

#define MOTOR_HZ        200
#define MOTOR_PWM_RES   8

constexpr uint8_t MOTOR1_CHANNEL = 0;
constexpr uint8_t MOTOR2_CHANNEL = 1;

constexpr uint16_t ESC_COAST_DIFF_US = 150;  // unused for now, keep for reference

static void setPower(uint8_t channel, double val) {
    val = constrain(val, -1.0, 1.0);
    ledcWrite(channel, 51 + ((val + 1.0) / 2.0) * 51);
}

void motor_1_on(float throttle) { setPower(MOTOR1_CHANNEL, throttle); }
void motor_2_on(float throttle) { setPower(MOTOR2_CHANNEL, throttle); }

void motor_1_coast()  { setPower(MOTOR1_CHANNEL,  0.0); }
void motor_2_coast()  { setPower(MOTOR2_CHANNEL,  0.0); }

void motor_1_off()    { setPower(MOTOR1_CHANNEL, 0.0); }
void motor_2_off()    { setPower(MOTOR2_CHANNEL, 0.0); }
void motors_off()     { motor_1_off(); motor_2_off(); }

void init_motors() {
    ledcSetup(MOTOR1_CHANNEL, MOTOR_HZ, MOTOR_PWM_RES);
    ledcAttachPin(MOTOR_PIN1, MOTOR1_CHANNEL);

    ledcSetup(MOTOR2_CHANNEL, MOTOR_HZ, MOTOR_PWM_RES);
    ledcAttachPin(MOTOR_PIN2, MOTOR2_CHANNEL);

    motors_off();
    delay(250);

    Serial.println("Motors initialised");
}