#include <Arduino.h>

#include "esp32_rc_handler.h"
#include "esp32_led_driver.h"
#include "accel_handler.h"
#include "motor_driver.h"
#include "watchdog.h"

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
float last_measurement_frequency = 0.0f;    // Hz (or revolutions per second) of the spin
float robot_direction = 0.0f;

static float accel_mount_radius_cm = DEFAULT_ACCEL_MOUNT_RADIUS_CM;
static float accel_zero_g_offset = DEFAULT_ACCEL_ZERO_G_OFFSET_X;

// loads settings from EEPROM
void load_melty_config_settings_esp32() {       // Avoid multiple definition of `load_melty_config_settings()`
#ifdef ENABLE_EEPROM_STORAGE
    accel_mount_radius_cm = load_accel_mount_radius();
    accel_zero_g_offset = load_accel_zero_g_offset();
#endif
}

void init_high_speed_control() {
    load_melty_config_settings_esp32();
    last_measurement_micros = micros();  // uint32_t overflows in 70 minutes.
}

// This function changes the direction of the heading LED
// interval: seconds that have passed since last call of this function
void revolution_motion(float interval) {
    if (config_mode) {
        /* config mode */
    } else {
        robot_direction += MAX_HEADING_LED_REVOLUTION_SPEED * revolution_ratio * interval;
    }
}

void update_robot_direction() {
    /* robot direction calculation - in SI units */
    float radial_acceleration = fabs(get_accel_force_g() - accel_zero_g_offset) * GRAVITATIONAL_ACCELERATION;
    float angular_velocity = sqrt(radial_acceleration / (accel_mount_radius_cm * 0.01f));       // radial_acceleration = angular_velocity^2 * radius
    float current_frequency = angular_velocity / (2.0f * PI);                                   // angular_velocity = 2 * PI * frequency

    /* Accumulate frequency to get robot direction */
    float interval = (micros() - last_measurement_micros) * 0.000001f;                          // convert to seconds
    last_measurement_micros = micros();
    robot_direction += (current_frequency + last_measurement_frequency) * interval / 2.0f;      // trapezoid estimation

    revolution_motion(interval);

    // Rescrict robot_direction only in [0.0, 1.0)
    while (robot_direction >= 1.0f) {
        robot_direction -= 1.0f;
    }
    while (robot_direction < 0.0f) {
        robot_direction += 1.0f;
    }
}

void translation_motion() {
    // Assuming motor 1 is left, motor 2 is right, robot_direction

}

void high_speed_set_motor() {
    service_watchdog();             // Watchdog is hungry
    update_robot_direction();
    update_led(robot_direction);    // Update LED

    // Update Motor
    translation_motion();
}