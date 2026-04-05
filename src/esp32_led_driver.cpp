#include <Arduino.h>
#include <FastLED.h>

#include "melty_config.h"
#include "config_storage.h"

// The following code treats the physical length of the LED strip as 1 (no unit)

// If LED strip is enabled, LED is controlled using SPI (SCL and Data)
// If LED strip isn't enabled, LED is controlled using digitalWrite()
#define ENABLE_LED_STRIP

#define NUM_LEDS 14         // The number of LEDs on one LED strip
#define COLOR_ORDER BGR     // Check physical channel order. BGR is common for SK9822; APA102 may vary
#define ARRAY_SIZE(x) (sizeof(x) / sizeof(x[0]))        // For array size calculation convenience and performance

CRGB leds[NUM_LEDS];

const int led_on_percent = 30;  // from 0 to 100
static int led_offset_percent = DEFAULT_LED_OFFSET_PERCENT;         // stored in EEPROM as an INT - but handled as a float for configuration purposes

/*
### Current pattern
- 0: sector
- 1: arrow
- 2: pizza
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
        (robot_direction * 100) > (led_offset_percent - led_on_percent / 2 + 100) % 100 &&
        (robot_direction * 100) < (led_offset_percent + led_on_percent / 2) % 100
    );
}

void change_pattern(int new_pattern) {
    current_led_pattern = new_pattern;
}

/* Render different patterns (draw svg) */
// Draw a sector
void draw_sector(float robot_direction) {
    if (calculate_led_status(robot_direction)) {
        // Turn all LEDs White
        fill_solid(leds, NUM_LEDS, CRGB::White);
    } else {
        // Turn all LEDs Black (= turn all LEDs off)
        fill_solid(leds, NUM_LEDS, CRGB::Black);
    }
}

// Draw an arrow
const float arrow_graphs[][2] = {       // The coordinates of the vertices of the shape
    { 0.00f,  0.90f },
    { 0.50f,  0.00f },
    { 0.15f,  0.15f },
    { 0.15f, -0.80f },
    {-0.15f, -0.80f },
    {-0.15f,  0.15f },
    {-0.50f,  0.00f }
};
#define arrow_vertex_num (int)ARRAY_SIZE(arrow_graphs)
void draw_arrow(float robot_direction) {

    for (int led_index = 0; led_index < NUM_LEDS; led_index++) {
        // LED coordinate
        float led_x = cos(2 * PI * (robot_direction + 0.25f));
        float led_y = sin(2 * PI * (robot_direction + 0.25f));

        /* Ray Casting Algorithm */
        int num_intersections = 0;
        for (int i = 0; i < arrow_vertex_num; i++) {
            // Vertex 1 coordinate
            float v1_x = arrow_graphs[i][0];
            float v1_y = arrow_graphs[i][1];
            // Vertex 2 coordinate
            float v2_x = arrow_graphs[(i + 1) % arrow_vertex_num][0];
            float v2_y = arrow_graphs[(i + 1) % arrow_vertex_num][1];

            // Check whether the ray horizontal to the right intersects the segment between Vertex 1 and Vertex 2
            if (
                (v1_y > led_y) != (v2_y > led_y) &&         // Two vertices have different y, and led is between them
                (led_x < (v2_x - v1_x) / (v2_y - v1_y) * (led_y - v1_y) + v1_x)
            ) {
                num_intersections++;
            }
        }
        // Check whether the LED is in the polygon
        if (num_intersections % 2 == 1) {   // The LED is inside the polygon
            leds[led_index] = CRGB::White;
        } else {                            // The LED is ouside the polygon
            leds[led_index] = CRGB::Black;
        }
    }
}

// Draw a pizza
#define pepperoni_diameter 0.25
const float pizza_pepperoni[][2] = {    // The coordinate of the center of the pepperonis
    { 0.00f,  0.90f },
    { 0.50f,  0.00f }
};
const float pizza_mushroom[][3] = {     // The coordinate and the orientation of the mushrooms
    { 0.00f,  0.90f,  0.00f },
    { 0.50f,  0.00f,  0.30f }
};
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