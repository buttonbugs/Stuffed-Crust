#ifndef ESP32_RC_HANDLER
#define ESP32_RC_HANDLER

extern float throttle_ratio;            // from 0.0 to 1.0 based on stick position
extern float forback_ratio;             // from -1.0 to 1.0 based on stick position
extern float leftright_ratio;           // from -1.0 to 1.0 based on stick position
extern float revolution_ratio;          // from -1.0 to 1.0 based on stick position
extern bool facing_up;                  // true/false based on the switch on the xbox controller

#endif