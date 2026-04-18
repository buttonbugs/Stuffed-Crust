#ifndef ESP32_LED_DRIVER
#define ESP32_LED_DRIVER

void init_led();

void change_pattern(int new_pattern);

// robot_direction is ranging from 0.0 to 1.0, representing the +y-axis direction of the robot (not the direction of the LED) in one revoluction
void update_led(float robot_direction);

void heading_led_on(int led_shimmering);

void heading_led_off();

#endif