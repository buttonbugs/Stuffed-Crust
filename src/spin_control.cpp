#include <Arduino.h>
#include "spin_control.h"

#include "rc_handler.h"
#include "led_driver.h"
#include "accel_handler.h"
#include "motor_driver.h"

#include "melty_config.h"
#include "config_storage.h"

/* 
    Purpose of this file:
        NOT TO use any "delay()" or "while loop for the whole revolution" in high_speed_set_motor()

    Usage:
        put high_speed_set_motor() inside the loop() in main.cpp
 */

#define GRAVITATIONAL_ACCELERATION 9.8f      // m / s^2
#define MOTOR_COAST_THROTTLE 0.3f
#define MAX_HEADING_LED_REVOLUTION_SPEED 0.8f   // revolutions per second, maximum heading LED revolution speed

uint32_t last_measurement_micros = 0;       // microseconds, the value of micros() when setting the last last_measurement_frequency
uint32_t interval_micros = 0;               // microseconds, interval of running update_robot_direction()
float last_measurement_frequency = 0.0f;    // Hz (or revolutions per second) of the spin
float robot_direction = 0.0f;
float current_frequency = 1.0f;

float accel_mount_radius_cm = DEFAULT_ACCEL_MOUNT_RADIUS_CM;
static float accel_zero_g_offset = DEFAULT_ACCEL_ZERO_G_OFFSET_X;

// loads settings from EEPROM
void load_melty_config_settings_esp32() {       // Avoid multiple definition of `load_melty_config_settings()`
#ifdef ENABLE_EEPROM_STORAGE
    accel_mount_radius_cm = load_accel_mount_radius();
    accel_zero_g_offset = load_accel_zero_g_offset();
#endif
}

void load_melty_config_settings() {
    load_melty_config_settings_esp32();
    last_measurement_micros = micros();  // uint32_t overflows in 70 minutes.
}

// Config Mode
void update_config() {
    #ifndef BOARD_TEENSY40
        if (config_left) {
            accel_mount_radius_cm -= 0.00002f;
        } else if (config_right) {
            accel_mount_radius_cm += 0.00002f;
        }
        if (config_heading) {
            led_offset_ratio += 0.0004f;
            led_offset_ratio -= floor(led_offset_ratio);    // Restrict led_offset_ratio in [0.0, 1.0)
        }
    #endif
}

// This function changes the direction of the heading LED
// interval: seconds that have passed since last call of this function
void rotational_motion(float interval) {
    if (config_mode) {
        /* config mode */
    } else {
        robot_direction += MAX_HEADING_LED_REVOLUTION_SPEED * revolution_ratio * interval;
    }
}

void translational_motion() {
    float motor_1_throttle = 1.0f;          // ranging from -1.0 to 1.0
    float motor_2_throttle = 1.0f;          // ranging from -1.0 to 1.0
    float translation_throttle = constrain(sqrt(sq(forback_ratio) + sq(leftright_ratio)), -1.0f, 1.0f); // the throttle of the translation joystick
    Serial.print("forback_ratio: ");
    Serial.print(forback_ratio);
    Serial.print("leftright_ratio: ");
    Serial.print(leftright_ratio);
    Serial.print("\n");
    float translation_direction;            // the direction of the translation joystick, using +y-axis as the 0 direction

    // Calculation translation direction, return from 0.0 to 1.0
    if (leftright_ratio == 0.0f) {
        if (forback_ratio > 0.0f) {
            translation_direction = 0.0f;
        } else {
            translation_direction = 0.5f;
        }
    } else {
        translation_direction = atan(forback_ratio / leftright_ratio) / (2 * PI);   // Use +x-axis as the 0 direction
        if (leftright_ratio < 0.0f) {
            translation_direction += 0.5f;
        }
        translation_direction -= 0.25f;     // Use +y-axis as the 0 direction
    }

    /* Check if the robot rotates to the translation direction */
    float direction_difference = robot_direction - translation_direction;
    /* 
    modulo direction_difference to [-0.5, 0.5)
    For example:
    1.6 -> -0.4
    1.4 -> +0.4
    0.9 -> -0.1
    0.4 -> +0.4
    -0.6 -> +0.4
    -1.2 -> -0.2
    -1.4 -> -0.4
    -1.6 -> +0.4
    */
    direction_difference -= floor(direction_difference + 0.5f);
    
    // take abosulte value
    direction_difference = abs(direction_difference);   // return [0.0, 0.5]

    // Set motor throttle
    if (direction_difference < 0.25f * translation_throttle) {
        // Assuming motor 1 is on the left, motor 2 is on the right
        motor_1_throttle = MOTOR_COAST_THROTTLE;
        motor_2_throttle = 2.0f * motor_2_throttle - MOTOR_COAST_THROTTLE;  // For symmetry, making robot more stable
    } else if (0.5f - direction_difference < 0.25f * translation_throttle) {
        // Assuming motor 1 is on the left, motor 2 is on the right
        motor_2_throttle = MOTOR_COAST_THROTTLE;
        motor_1_throttle = 2.0f * motor_1_throttle - MOTOR_COAST_THROTTLE;  // For symmetry, making robot more stable
    }
    
    motor_1_throttle *= throttle_ratio;
    motor_2_throttle *= throttle_ratio;
    motor_1_throttle = constrain(motor_1_throttle, -1.0f, 1.0f);
    motor_2_throttle = constrain(motor_2_throttle, -1.0f, 1.0f);
    motor_1_on(motor_1_throttle);
    motor_2_on(motor_2_throttle);
}

void update_robot_direction() {
    /* robot direction calculation - in SI units */
    float radial_acceleration = fabs(get_accel_force_g() - accel_zero_g_offset) * GRAVITATIONAL_ACCELERATION;
    float angular_velocity = sqrt(radial_acceleration / (accel_mount_radius_cm * 0.01f));       // radial_acceleration = angular_velocity^2 * radius
    current_frequency = angular_velocity / (2.0f * PI);                                   // angular_velocity = 2 * PI * frequency

    /* Accumulate frequency to get robot direction */
    interval_micros = micros() - last_measurement_micros;
    float interval = interval_micros * 0.000001f;                          // convert to seconds
    last_measurement_micros = micros();

    if (facing_up) {
        robot_direction += (current_frequency + last_measurement_frequency) * interval / 2.0f;      // trapezoid estimation with positive area
    } else {
        robot_direction -= (current_frequency + last_measurement_frequency) * interval / 2.0f;      // trapezoid estimation with negative area
    }

    rotational_motion(interval);

    // Rescrict robot_direction into [0.0, 1.0)
    robot_direction -= floor(robot_direction);
}

void high_speed_set_motor() {
    service_watchdog();             // Watchdog is hungry
    update_robot_direction();
    update_led(robot_direction);    // Update LED

    // Update Motor
    translational_motion();

    // Config Mode
    if (config_mode) {
        update_config();
    }
}