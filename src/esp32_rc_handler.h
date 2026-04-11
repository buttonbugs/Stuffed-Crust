#ifndef ESP32_RC_HANDLER
#define ESP32_RC_HANDLER

extern float throttle_ratio;            // from 0.0 to 1.0 based on stick position
extern float forback_ratio;             // from -1.0 to 1.0 based on stick position
extern float leftright_ratio;           // from -1.0 to 1.0 based on stick position
extern float revolution_ratio;          // from -1.0 to 1.0 based on stick position
extern bool facing_up;                  // true/false based on the switch on the xbox controller
extern bool config_mode;                // true/false based on the switch on the xbox controller

void get_data_from_external_rc();       // if external receiver instead of ESP32 is used as the receiver

#endif