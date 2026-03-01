#include "rc_handler.h"
#include "motor_driver.h"

void low_speed_set_motor() {
    //
    rc_get_forback_ratio();
    rc_get_leftright_ratio();
    rc_get_revolution_ratio();
    rc_get_throttle_ratio();
    // And then controll the motors according to the four numbers above.
}