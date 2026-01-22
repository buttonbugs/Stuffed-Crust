# Stuffed Crust

This repository contains the code for Wildcat Robotics' team Stuffed Crust. This is a fork of the [open source melty brain code](https://github.com/nothinglabs/openmelt2/tree/main), openmelt, with some changes specific to Stuffed Crust's design.

TODO:
- Add Mecanum Controls
- Replace all `#include "arduino.h"` and `#include <arduino.h>` with `#include <Arduino.h>`
- Change `#define ACCEL_I2C_ADDRESS 0x18` in `accel_handler.h` to `#define ACCEL_I2C_ADDRESS 0x19` (Adafuit breakout default is 0x18, Sparkfun default is 0x19, and we use Sparkfun)
- The Following 3 lines in `melty_config.h` should be changed according to the wire graph
```
#define LEFTRIGHT_RC_CHANNEL_PIN 20                // Change 20 into 23
#define FORBACK_RC_CHANNEL_PIN 21                  // Change 21 into 22
#define THROTTLE_RC_CHANNEL_PIN 22                 // Change 22 into 21
```
![Wiring.png](Wiring.png)

