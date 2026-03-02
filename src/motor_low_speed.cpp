#include "melty_config.h"
#include "rc_handler.h"
#include "motor_driver.h"

void low_speed_set_motor() {
    float forback_ratio = rc_get_forback_ratio();
    float revolution_ratio = rc_get_revolution_ratio();

    // The following parameters might be negative. Please turn on ENABLE_REVERSE in melty_config.h before calling low_speed_set_motor()
    motor_1_on(forback_ratio * LOW_SPEED_FORBACK_THROTTLE_PERCENT - revolution_ratio * LOW_SPEED_REVOLUTION_THROTTLE_PERCENT);
    motor_2_on( - forback_ratio * LOW_SPEED_FORBACK_THROTTLE_PERCENT - revolution_ratio * LOW_SPEED_REVOLUTION_THROTTLE_PERCENT);
}