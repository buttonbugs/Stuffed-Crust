// this module interfaces with the accelerometer to provide current G-force level

// Two breakout boards using the H3LIS331DL accelerometer have been tested / verified to work

// Sparkfun's H3LIS331DL breakout: https://www.sparkfun.com/products/14480 (3v part)
// Requires using  to interface with 5v arduino

// Adafruit's HSLIS311 breakout includes a level converter - is a single component solution:
// https://www.adafruit.com/product/4627

// Update ACCEL_I2C_ADDRESS in accel_handler.h with I2c address (Adafruit and Sparkfun boards are different!)

// Sparkfun's LIS311 library is used (also works for Adafruit part)

#include <Arduino.h>
#include "melty_config.h"
#include "accel_handler.h"
#include <Wire.h>

LIS331 xl;
bool is_facing_up = true;   // true: facing up, false: upside down
float gyro_z;

void init_accel() {
#if defined(BOARD_ESP32C3)
    // ESP32-C3: Configure I2C with custom pins (SDA=10, SCL=9)
    Wire.begin(6, 7);
#else
    // Teensy 4.0: Use default I2C pins
    Wire.begin();
#endif

    Wire.setClock(400000);  // increase I2C speed to reduce read times a bit
                            // value of 400000 allows accel read in ~1ms and
                            // is verfied to work with Sparkfun level converter
                            // (some level converters have issues at higher speeds)

    xl.setI2CAddr(ACCEL_I2C_ADDRESS);
    xl.begin(LIS331::USE_I2C);

    // sets accelerometer to specified scale (100, 200, 400g)
    xl.setFullScale(ACCEL_RANGE);
}

// reads accel and converts to G's
// ACCEL_MAX_SCALE needs to match ACCEL_RANGE value
float get_accel_force_g() {
    int16_t x, y, z;
    xl.readAxes(x, y, z);
    gyro_z = xl.convertToG(ACCEL_MAX_SCALE, z) - DEFAULT_ACCEL_ZERO_G_OFFSET_Z;
    is_facing_up = gyro_z > 0;
    return xl.convertToG(ACCEL_MAX_SCALE, x);
}