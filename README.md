# Stuffed Crust

This repository contains the code for Wildcat Robotics' team Stuffed Crust. This is a fork of the [open source melty brain code](https://github.com/nothinglabs/openmelt2/tree/main), openmelt, with some changes specific to Stuffed Crust's design.

TODO:
- Add Mecanum Controls
- Replace all `#include "arduino.h"` and `#include <arduino.h>` with `#include <Arduino.h>`
- Change `#define ACCEL_I2C_ADDRESS 0x18` in `accel_handler.h` to `#define ACCEL_I2C_ADDRESS 0x19` (Adafuit breakout default is 0x18, Sparkfun default is 0x19, and we use Sparkfun)
![Wiring.png](Wiring.png)
