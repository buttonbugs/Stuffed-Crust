#include "watchdog.h"       // No need to use #ifdef ENABLE_WATCHDOG here - handled in watchdog.h

//does translational drift rotation (robot spins 360 degrees)
void spin_one_rotation(void);

//returns maximum rotation speed since last entering config mode
int get_max_rpm();

//toggles configuration mode
void toggle_config_mode();

//saves configuration settings
void save_melty_config_settings();