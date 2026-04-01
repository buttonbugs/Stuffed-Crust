#include <Arduino.h>
#include <FastLED.h>

#include "melty_config.h"
#include "config_storage.h"

// If LED strip is enabled, LED is controlled using SPI (SCL and Data)
// If LED strip isn't enabled, LED is controlled using digitalWrite()
// #define ENABLE_LED_STRIP

#define NUM_LEDS 30         // The number of LEDs on one LED strip
#define COLOR_ORDER BGR     // Check physical channel order. BGR is common for SK9822; APA102 may vary
CRGB leds[NUM_LEDS];

const int led_on_percent = 30;  // from 0 to 100
static int led_offset_percent = DEFAULT_LED_OFFSET_PERCENT;         // stored in EEPROM as an INT - but handled as a float for configuration purposes

/*
### Current pattern
- 0: sector
 */
int current_led_pattern = 0;

void init_led_driver() {
    // Get led_offset_percent from storage
    #ifdef ENABLE_EEPROM_STORAGE
        led_offset_percent = load_heading_led_offset();
    #endif

    // Initialize LED
    #ifdef ENABLE_LED_STRIP
        FastLED.addLeds<APA102, LED_STRIP_DATA, LED_STRIP_SCK, COLOR_ORDER>(leds, NUM_LEDS);
        FastLED.setBrightness(50);  // Set initial brightness (0-255)
    #else
        pinMode(HEADING_LED_PIN, OUTPUT);
    #endif
}

bool calculate_led_status(float robot_direction) {
    return (
        (robot_direction * 100) > (100 + led_offset_percent - led_on_percent / 2) % 100 &&
        (robot_direction * 100) < (led_offset_percent + led_on_percent / 2) % 100
    );
}

void change_pattern(int new_pattern) {
    current_led_pattern = new_pattern;
}

/* Render different patterns (draw svg) */
void draw_sector(float robot_direction) {
    if (calculate_led_status(robot_direction)) {
        // Turn all LEDs White
        fill_solid(leds, NUM_LEDS, CRGB::White);
    } else {
        // Turn all LEDs Black (= turn all LEDs off)
        fill_solid(leds, NUM_LEDS, CRGB::Black);
    }
}

void draw_arrow(float robot_direction) {
    //
}

void draw_pizza(float robot_direction) {
    // Just for fun
}

/* Update pixel */
// This function should be called in high_speed_set_motor() in esp32_spin_control.cpp
// robot_direction is ranging from 0.0 to 1.0, representing the direction of the robot (not the direction of the led) in on revoluction
void update_led(float robot_direction) {
    #ifdef ENABLE_LED_STRIP
        // Caculate each pixel on the line
        switch (current_led_pattern) {
        case 1:
            draw_arrow(robot_direction);
            break;
        
        case 2:
            draw_pizza(robot_direction);
            break;
        
        default:
            draw_sector(robot_direction);
            break;

        }
        FastLED.show();
    #else
        // Caculate whether the LED should be ON or OFF
        digitalWrite(HEADING_LED_PIN, calculate_led_status(robot_direction));
    #endif
    
}