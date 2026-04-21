#include "melty_config.h"

// Fix macro collision with ESP32 framework
// ESP32 defines OPEN_DRAIN as 0x10, but SparkFun library uses it as an enum value
#ifdef OPEN_DRAIN
#undef OPEN_DRAIN
#endif

#include "SparkFun_LIS331.h"

//Set high enough to allow for G forces at top RPM
//LOW_RANGE - +/-100g for the H3LIS331DH
//MED_RANGE - +/-200g for the H3LIS331DH
//HIGH_RANGE - +/-400g for the H3LIS331DH
#define ACCEL_RANGE LIS331::MED_RANGE   

//Set to correspond to ACCEL_RANGE
#define ACCEL_MAX_SCALE 200

//Change as needed as needed
//(Adafuit breakout default is 0x18, Sparkfun default is 0x19)
#define ACCEL_I2C_ADDRESS 0x19

extern bool is_facing_up;
extern float gyro_z;

void init_accel();

float get_accel_force_g();

