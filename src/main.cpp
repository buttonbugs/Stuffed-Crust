// See melty_config.h for configuration parameters

#include <Arduino.h>
#include "rc_handler.h"
#include "melty_config.h"
#include "motor_driver.h"
#include "accel_handler.h"
#include "spin_control.h"
#include "motor_low_speed.h"
#include "config_storage.h"
#include "led_driver.h"
#include "battery_monitor.h"

#ifndef BOARD_TEENSY40
    #include "data_relay.h"
#endif

// loops until a good RC signal is detected and throttle is zero (assures safe start)
static void wait_for_rc_good_and_zero_throttle() {
    while (rc_signal_is_healthy() == false || rc_get_throttle_percent() > 0) {
        //"slow on/off" for LED while waiting for signal
        heading_led_on(0);
        delay(250);
        heading_led_off();
        delay(250);

        // services watchdog and echo diagnostics while we are waiting for RC signal
        service_watchdog();
    }
}

// INITIAL SETUP
void setup() {
    Serial.begin(115200);

    // get motor drivers setup (and off!) first thing
    init_motors();
    init_led();

    // returns actual watchdog timeout MS
    init_watchdog();

    init_rc();
    init_accel();  // accelerometer uses i2c - which can fail blocking (so only initializing it -after- the watchdog is running)
    #ifndef BOARD_TEENSY40
        start_accel_task();  // start accelerometer task immediately after init
    #endif

#ifdef DATA_RELAY
    init_relay();
#endif

// load settings on boot
#ifdef ENABLE_EEPROM_STORAGE
    load_melty_config_settings();
#endif

// if JUST_DO_DIAGNOSTIC_LOOP - then we just loop and display debug info via USB (good for testing)
#ifdef JUST_DO_DIAGNOSTIC_LOOP
    while (1) {
        service_watchdog();
        echo_diagnostics();
        delay(250);  // delay prevents serial from getting flooded (can cause issues programming)
    }
#endif

#ifdef VERIFY_RC_THROTTLE_ZERO_AT_BOOT
    wait_for_rc_good_and_zero_throttle();  // Wait for good RC signal at zero throttle
    delay(MAX_MS_BETWEEN_RC_UPDATES + 1);  // Wait for first RC signal to have expired
    wait_for_rc_good_and_zero_throttle();  // Verify RC signal is still good / zero throttle
#endif
}

// dumps out diagnostics info
static void echo_diagnostics() {
    Serial.print("Raw Accel G: ");
    Serial.print(get_accel_force_g());
    
    Serial.print("  RC Health: ");
    Serial.print(rc_signal_is_healthy());
    
    Serial.print("  RC Throttle: ");
    Serial.print(rc_get_throttle_percent());
    
    Serial.print("  RC L/R: ");
    Serial.print(rc_get_leftright());
    
    Serial.print("  RC F/B: ");
    Serial.print(rc_get_forback());

#ifdef BATTERY_ALERT_ENABLED
    Serial.print("  Battery Voltage: ");
    Serial.print(get_battery_voltage());
#endif

#ifdef ENABLE_EEPROM_STORAGE
    Serial.print("  Accel Radius: ");
    Serial.print(load_accel_mount_radius());

    Serial.print("  Heading Offset: ");
    Serial.print(load_heading_led_offset());

    Serial.print("  Zero G Offset: ");
    Serial.print(load_accel_zero_g_offset());

    Serial.print("Gyro Z: ");
    Serial.print(gyro_z);
    
#endif
    Serial.println("");
}

// Used to flash out max recorded RPM 100's of RPMs
static void display_rpm_if_requested() {
    // #ifdef BOARD_TEENSY40
    // // triggered by user pushing throttle up while bot is at idle for 750ms
    // if (rc_get_forback() == RC_FORBACK_FORWARD) {
    //     delay(750);
    //     // verify throttle at zero to prevent accidental entry into RPM flash
    //     if (rc_get_forback() == RC_FORBACK_FORWARD && rc_get_throttle_percent() == 0) {
    //         // throttle up cancels RPM count
    //         for (int x = 0; x < get_max_rpm() && rc_get_throttle_percent() == 0; x = x + 100) {
    //             service_watchdog();  // flashing out RPM can take a while - need to assure watchdog doesn't trigger
    //             delay(600);
    //             heading_led_on(0);
    //             delay(20);
    //             heading_led_off();
    //         }
    //         delay(1500);  // flash-out punctuated with delay to make clear RPM count has completed
    //     }
    // }
    // #endif
}

// checks if user has requested to enter / exit config mode
static void check_config_mode() {
    // #ifdef BOARD_TEENSY40
    // // if user pulls control stick back for 150ms - enters (or exits) interactive configuration mode
    // if (rc_get_forback() == RC_FORBACK_BACKWARD) {
    //     delay(150);
    //     if (rc_get_forback() == RC_FORBACK_BACKWARD) {
    //         toggle_config_mode();
    //         if (get_config_mode() == false)
    //             save_melty_config_settings();  // save melty settings on config mode exit

    //         // wait for user to release stick - so we don't re-toggle modes
    //         while (rc_get_forback() == RC_FORBACK_BACKWARD) {
    //             service_watchdog();
    //         }
    //     }
    // }
    // #endif
}

// handles the bot when not spinning (with RC good)
static void handle_bot_idle() {
    motors_off();  // assure motors are off

    // normal LED "fast flash" - indicates RC signal is good while sitting idle
    heading_led_on(0);
    delay(30);
    heading_led_off();
    delay(120);

    // if in config mode blip LED again to show "double-flash"
    // if (get_config_mode() == true) {
    //     heading_led_off();
    //     delay(400);
    //     heading_led_on(0);
    //     delay(30);
    //     heading_led_off();
    //     delay(140);
    // }

    check_config_mode();         // check if user requests we enter / exit config mode
    display_rpm_if_requested();  // flashed out RPM if user has requested

    echo_diagnostics();  // echo diagnostics if bot is idle
}

// main control loop
void loop() {
    service_watchdog();  // keep the watchdog happy

    service_rc();
    #ifdef DATA_RELAY
        if(config_mode){
        #ifdef BOARD_TEENSY40
        relay_data({
            (uint8_t)rc_get_throttle_percent(), 
            (uint8_t)get_battery_voltage(), 
            (uint8_t)(get_accel_force_g()),  // convert to 0-100 scale (with 1 decimal place)
            0,0,0
        });
        #else
        relay_data({
            (uint8_t)rc_get_throttle_percent(), 
            (uint8_t)get_battery_voltage(),
            0,
            (uint8_t)(get_accel_force_g()),  // convert to 0-100 scale (with 1 decimal place)
            0,(uint16_t)(current_frequency * 60.0f),
            (float)accel_mount_radius_cm,
            (uint32_t)interval_micros,
            (int)(led_offset_ratio * 100.0f)
        });
        #endif
        } else {
            Serial.print("Interval: ");
            Serial.println(interval_micros);

            Serial.print("Current Frequency (RPS): ");
            Serial.println(current_frequency);

            Serial.print("Current LED Offset (%): ");
            Serial.println(led_offset_ratio * 100);
        }
    #endif

    // if the rc signal isn't good - assure motors off - and "slow flash" LED
    // this will interrupt a spun-up bot if the signal becomes bad
    while (rc_signal_is_healthy() == false) {
        motors_off();

        heading_led_on(0);
        delay(30);
        heading_led_off();
        delay(600);

        // services watchdog and echo diagnostics while we are waiting for RC signal
        service_watchdog();
        echo_diagnostics();
    }

    // if RC is good - and throtte is above 0 - spin a single rotation
    if (rc_get_throttle_percent() > LOW_SPEED_RC_THROTTLE_THRESHOLD) {
        #ifdef ENABLE_ESP32_SPIN_CONTROL
            high_speed_set_motor();
        #else
            // this is where all the motor control happens!  (see spin_control.cpp)
            spin_one_rotation();
        #endif
    } else {
        #ifdef BOARD_TEENSY40
            if (rc_get_throttle_percent() < 5) {
                handle_bot_idle();
            }
        #else
            bool current_led_status = (millis() % 150 < 30);
            digitalWrite(HEADING_LED_1_PIN, current_led_status);     // Blink the LED the same way as handle_bot_idle() without using delay()
            digitalWrite(HEADING_LED_2_PIN, current_led_status);     // Blink the LED the same way as handle_bot_idle() without using delay()
        #endif

        low_speed_set_motor();
    }
}