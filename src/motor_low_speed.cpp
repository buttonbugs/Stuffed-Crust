#include <Arduino.h>

#include "melty_config.h"
#include "rc_handler.h"
#include "motor_driver.h"

void low_speed_set_motor() {
    Serial.print("Low Speed Control");

    float local_leftright_ratio = pow(leftright_ratio, 3.0f);
    if (!facing_up) {
        local_leftright_ratio = -local_leftright_ratio;
    }

    // The following parameters might be negative. Please turn on ENABLE_REVERSE in melty_config.h before calling low_speed_set_motor()
    motor_1_on(forback_ratio * LOW_SPEED_FORBACK_THROTTLE_PERCENT + local_leftright_ratio * LOW_SPEED_REVOLUTION_THROTTLE_PERCENT);
    motor_2_on( - forback_ratio * LOW_SPEED_FORBACK_THROTTLE_PERCENT + local_leftright_ratio * LOW_SPEED_REVOLUTION_THROTTLE_PERCENT);
}