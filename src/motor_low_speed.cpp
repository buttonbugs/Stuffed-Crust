#include <Arduino.h>

#include "melty_config.h"
#include "rc_handler.h"
#include "motor_driver.h"
#include "accel_handler.h"

void low_speed_set_motor() {
    // Return ratio from -1.0 to 1.0
    float forback_ratio = rc_get_forback_ratio();
    float leftright_ratio = pow(rc_get_leftright_ratio(), 3.0f);

    /* if (!is_facing_up) {
        leftright_ratio = -leftright_ratio;
    } */

    // The following parameters might be negative. Please turn on ENABLE_REVERSE in melty_config.h before calling low_speed_set_motor()
    motor_1_on(forback_ratio * LOW_SPEED_FORBACK_THROTTLE_PERCENT + leftright_ratio * LOW_SPEED_REVOLUTION_THROTTLE_PERCENT);
    motor_2_on( - forback_ratio * LOW_SPEED_FORBACK_THROTTLE_PERCENT + leftright_ratio * LOW_SPEED_REVOLUTION_THROTTLE_PERCENT);
}