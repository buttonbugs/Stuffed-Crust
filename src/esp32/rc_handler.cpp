// ESP32 RC Handler using Bluepad32 for Xbox Controller support
// https://github.com/ricardoquesada/bluepad32
// Requires Bluepad32 board package for ESP32

#include <Arduino.h>
#include "melty_config.h"
#include "rc_handler.h"
#include "accel_handler.h"      // Only enable facing toggling when G < 20

#include <Bluepad32.h>

#define joystick_deadzone 0.1f          // range: 0 to 1.0f
#define throttle_deadzone 10            // range: 0 to 1023

// RC channel values (mapped from Xbox controller)
float throttle_ratio = 0.0;             // 0.0 to 1.0 (right trigger)
float forback_ratio = 0.0;              // -1.0 to 1.0 (left stick Y)
float leftright_ratio = 0.0;            // -1.0 to 1.0 (left stick X)
float revolution_ratio = 0.0;           // -1.0 to 1.0 (right stick X)
bool facing_up = true;
bool config_mode = false;
bool config_left = false;
bool config_right = false;
bool config_heading = false;
static volatile bool gamepad_connected = false;
static ControllerPtr myController = nullptr;

// Signal health tracking
static unsigned long last_good_signal = 0;
static const unsigned long SIGNAL_TIMEOUT_MS = 500;  // 500ms timeout

// Button cooling
// const int button_cooling_time_ms = 100;
// int left_bumper_last_trigger_ms = 0;
// bool left_bumper_status = false;

void onConnectedGamepad(ControllerPtr ctl) {
    Serial.print("Gamepad connected: index=");
    Serial.print(ctl->index());
    Serial.print(" model=");
    Serial.print(ctl->getModel(), 16);
    last_good_signal = millis();
    gamepad_connected = true;
    myController = ctl;
}

void onDisconnectedGamepad(ControllerPtr ctl) {
    Serial.print("Gamepad disconnected: index=");
    Serial.println(ctl->index());
    gamepad_connected = false;
    myController = nullptr;
    throttle_ratio = 0.0;
    forback_ratio = 0.0;
    leftright_ratio = 0.0;
    revolution_ratio = 0.0;
    config_mode = false;
    config_left = false;
    config_right = false;
    config_heading = false;
    // left_bumper_status = false;
}

void processGamepadData() {
    if (!myController || !gamepad_connected) {
        Serial.println("[RC] No gamepad connected - skipping data processing");
        return;
    }

    last_good_signal = millis();

    // Get analog stick values (axis range: -512 to 512)
    int16_t axisX = myController->axisX();       // Left stick X
    int16_t axisY = myController->axisY();       // Left stick Y
    int16_t axisRX = myController->axisRX();     // Right stick X
    int16_t axisRY = myController->axisRY();     // Right stick Y (not used currently)

    // Normalize to -1.0 to 1.0
    forback_ratio = -axisY / 512.0f;             // Forward/backward (Forward should be changed to positive)
    leftright_ratio = axisX / 512.0f;            // Left/right
    revolution_ratio = axisRX / 512.0f;          // Revolution/spin

    // Joystick Deadzone
    if (abs(forback_ratio) < joystick_deadzone) {
        forback_ratio = 0.0f;
    }
    if (abs(leftright_ratio) < joystick_deadzone) {
        leftright_ratio = 0.0f;
    }
    if (abs(revolution_ratio) < joystick_deadzone) {
        revolution_ratio = 0.0f;
    }

    // Get trigger values (range: 0 to 1023)
    uint32_t throttleL = myController->brake();  // Left trigger
    uint32_t throttleR = myController->throttle();  // Right trigger

    // Throttle Deadzone
    if (throttleL < throttle_deadzone) {
        throttleL = 0;
    }
    if (throttleR < throttle_deadzone) {
        throttleR = 0;
    }

    // Map triggers to throttle (0.0 to 1.0)
    throttle_ratio = max(throttleL, throttleR) / 1023.0f;
    throttle_ratio = constrain(throttle_ratio, 0.0f, 1.0f);

    if (throttleL < throttleR) {
        facing_up = true;
    } else if (throttleL > throttleR) {
        facing_up = false;
    }   // When throttleL == throttleR, do not change the value of facing_up

    // Get button states
    uint16_t buttons = myController->buttons();
    uint8_t dpad = myController->dpad();
    
    // if (millis() - left_bumper_last_trigger_ms > button_cooling_time_ms) {  // Toggle facing_up
    //     bool left_bumper_current_status = myController->l1();
    
    //     if (left_bumper_current_status != left_bumper_status) {
    //         left_bumper_status = left_bumper_current_status;
    //         left_bumper_last_trigger_ms = millis();
            
    //         if (left_bumper_status) {
    //             if (abs(get_accel_force_g()) < 20.0f) {     // 20 G is much more than the offset, so G offset is ignored. Otherwise, G offset is too complicated to add here
    //                 facing_up = !facing_up;
    //             }
    //         }
    //     }
    // }
    
    // Map buttons (using button bitmasks)
    config_mode = (dpad & DPAD_DOWN);     // B button for config mode
    config_left = (dpad & DPAD_LEFT);     // B button for config mode
    config_right = (dpad & DPAD_RIGHT);     // B button for config mode
    config_heading = myController -> r1();

    // Debug output every 500ms
    static unsigned long last_print = 0;
    if (millis() - last_print > 500) {
        Serial.print("Gamepad connected - Throttle: ");
        Serial.print(throttle_ratio, 3);
        Serial.print(" FB: ");
        Serial.print(forback_ratio, 3);
        Serial.print(" LR: ");
        Serial.print(leftright_ratio, 3);
        Serial.print(" Rev: ");
        Serial.print(revolution_ratio, 3);
        Serial.print(" Config: ");
        Serial.println(config_mode ? "YES" : "NO");
        last_print = millis();
    }
}

void init_rc() {
    Serial.println("[RC] Initializing Bluepad32 RC Handler");
    
    // Initialize Bluepad32 with callback functions
    Serial.println("[RC] Calling BP32.setup()...");
    BP32.setup(&onConnectedGamepad, &onDisconnectedGamepad);
    Serial.println("[RC] BP32.setup() complete");
    
    // Force scan to start
    Serial.println("[RC] Starting Bluepad32 scan for gamepads...");
    BP32.update();
    Serial.println("[RC] Ready - waiting for gamepad connection");
}

// service_rc() - Update gamepad data
void service_rc() {
    // Update Bluepad32 (processes gamepad events and calls callbacks)
    BP32.update();
    
    // Process any connected gamepad data
    processGamepadData();
    
    // Debug output every 2 seconds showing connection status
    static unsigned long last_status_print = 0;
    if (millis() - last_status_print > 2000) {
        ControllerPtr ctl = GamepadPtr(0);
        Serial.print("[RC] BP32 Scanning...");
        if (ctl && ctl->isConnected()) {
            Serial.println(" Connected!");
        } else {
            Serial.println(" (no gamepad found)");
        }
        last_status_print = millis();
    }
}

bool rc_signal_is_healthy() {
    BP32.update();
    Serial.println(gamepad_connected ? "[RC] Gamepad is connected" : "[RC] No gamepad connected");
    return gamepad_connected && myController->isConnected();
} 

float rc_get_throttle_percent() {
    Serial.print("[RC] Throttle ratio: ");
    Serial.println(throttle_ratio);
    return throttle_ratio * 100.0f;  // Convert to 0-100%
}

float rc_get_forback_ratio() {
    return forback_ratio;  // -1.0 to 1.0
}

float rc_get_leftright_ratio() {
    return leftright_ratio;  // -1.0 to 1.0
}

float rc_get_revolution_ratio() {
    return revolution_ratio;  // -1.0 to 1.0
}

bool rc_get_config_mode() {
    return config_mode;
}

bool rc_get_facing_direction() {
    return facing_up;
}

// ===== Teensy Compatibility Wrapper Functions =====
// These functions convert ESP32 ratio values to Teensy enum format for compatibility

// Convert forback ratio to enum
rc_forback rc_get_forback() {
    if (forback_ratio > 0.2f) return RC_FORBACK_FORWARD;
    if (forback_ratio < -0.2f) return RC_FORBACK_BACKWARD;
    return RC_FORBACK_NEUTRAL;
}

// Convert leftright ratio to enum  
rc_forback rc_get_leftright() {
    if (leftright_ratio > 0.2f) return RC_FORBACK_FORWARD;
    if (leftright_ratio < -0.2f) return RC_FORBACK_BACKWARD;
    return RC_FORBACK_NEUTRAL;
}

// Get config mode (Teensy format)
bool get_config_mode() {
    return config_mode;
}