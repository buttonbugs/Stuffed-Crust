#include <Arduino.h>

#include "melty_config.h"


// If LED strip is enabled, LED is controlled using SPI (SCL and Data)
// If LED strip isn't enabled, LED is controlled using digitalWrite()
// #define ENABLE_LED_STRIP

void init_led_driver() {
    #ifdef ENABLE_LED_STRIP

    #else
    pinMode(HEADING_LED_PIN, OUTPUT);
    #endif
}

/* Render different patterns (drawing pictures) */
void draw_line() {
    //
}

void draw_arrow() {
    //
}

void draw_pizza() {
    // Just for fun
}

/* Update pixel */
// This function should be called in high_speed_set_motor() in esp32_spin_control.cpp
// robot_direction is ranging from 0.0 to 1.0, representing the direction of the robot (not the direction of the led) in on revoluction
void update_led(float robot_direction) {
    #ifdef ENABLE_LED_STRIP
    // Caculate each pixel on the line

    #else
    // Caculate whether the LED should be ON or OFF
    const int led_on_percent = 40;  // from 0 to 100
    digitalWrite(
        HEADING_LED_PIN,
        (robot_direction * 100) > (100 + DEFAULT_LED_OFFSET_PERCENT - led_on_percent / 2) % 100 &&
        (robot_direction * 100) < (DEFAULT_LED_OFFSET_PERCENT + led_on_percent / 2) % 100
    );
    #endif
    
}