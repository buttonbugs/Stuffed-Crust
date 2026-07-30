#include <Arduino.h>
#include "melty_config.h"
#include "accel_handler.h"
#include <Wire.h>

LIS331 xl;

// Shared state - volatile so compiler doesn't cache in register
static volatile float cached_accel_g = 0.0f;
static volatile uint32_t last_read_us = 0;  // optional: lets you detect stale data

static TaskHandle_t accel_task_handle = NULL;
float gyro_z;

void init_accel() {
    Wire.begin(6, 7);
    Wire.setClock(400000);

    xl.setI2CAddr(ACCEL_I2C_ADDRESS);
    xl.begin(LIS331::USE_I2C);
    xl.setFullScale(ACCEL_RANGE);
}

// Runs on Core 0 - hammers I2C as fast as possible
static void accel_task(void *param) {
    int16_t x, y, z;

    while (true) {
        xl.readAxes(x, y, z);
        cached_accel_g = xl.convertToG(ACCEL_MAX_SCALE, y);
        last_read_us = micros();
        // No delay - run as fast as 400kHz I2C allows (~1ms per read)
    }
}

void start_accel_task() {
    xTaskCreatePinnedToCore(
        accel_task,         // task function
        "accel_task",       // name (for debugging)
        2048,               // stack size in bytes - increase if you see stack overflows
        NULL,               // parameters
        1,                  // priority (1 = low, leaves headroom for system tasks)
        &accel_task_handle, // handle
        0                   // pin to Core 0
    );
}

// Safe to call from ISR or heading loop on Core 1 - just returns cached value
float get_accel_force_g() {
    return cached_accel_g;
}

// Optional: lets heading loop detect if accel task has stalled
uint32_t get_accel_age_us() {
    return micros() - last_read_us;
}