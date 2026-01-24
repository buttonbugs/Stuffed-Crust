typedef struct rc_translation {
    int forback;    // Percent -100 (backward) to 0 (neutral) to +100 (forward)
    int leftright;  // Percent -100 (left) to 0 (neutral) to +100 (right)
} rc_translation;

void init_rc();

bool rc_signal_is_healthy();           //return true if RC signal looks good

int rc_get_throttle_percent();        //returns 0-100 value indicating throttle level

rc_translation rc_get_translation();
int rc_get_revolution();               //returns offset in microseconds from center value (not converted to percentage)

//these functions return true if L/R stick movement is below defined thresholds
bool rc_get_is_rev_in_config_deadzone();  
bool rc_get_is_rev_in_normal_deadzone();

//All pulse lengths in microseconds
//it's accepted that a TX with fully centered trims may produce values somewhat off these numbers

//RC pulses outside this range are considered invalid (indicate a bad RC signal)
#define MAX_RC_PULSE_LENGTH 2100
#define MIN_RC_PULSE_LENGTH 900

//This value reflects nominal range of possible RC pulse values (maximum - minimum)
//This value is used to help scale left / right adjustment of heading
//(does not need to be perfect)
#define NOMINAL_PULSE_RANGE (MAX_RC_PULSE_LENGTH - MIN_RC_PULSE_LENGTH)

#define IDLE_THROTTLE_PULSE_LENGTH 1250           //pulses below this value are considered 0% throttle
#define FULL_THROTTLE_PULSE_LENGTH 1850           //pulses above this value are considered 100%
#define CENTER_REVOLUTION_PULSE_LENGTH 1500        //center value for left / right

#define CENTER_FORBACK_PULSE_LENGTH 1500         //center value for forward / backward
#define CENTER_LEFTRIGHT_PULSE_LENGTH 1500       //center value for left / right

#define FORBACK_MIN_THRESH_PULSE_LENGTH 100       //pulse length must differ by this much from CENTER_FORBACK_PULSE_LENGTH to be considered going forward or back
#define LEFTRIGHT_MIN_THRESH_PULSE_LENGTH 100       //pulse length must differ by this much from CENTER_LEFTRIGHT_PULSE_LENGTH to be considered going forward or back

#define REV_CONFIG_MODE_DEADZONE_WIDTH 100         //deadzone for REV when in config mode (in US) - prevents unintended tracking adjustments
#define REV_NORMAL_DEADZONE_WIDTH 25               //deadzone for normal drive - can help with unintentional drift when moving forward / back

#define MAX_MS_BETWEEN_RC_UPDATES 900             //if we don't get a valid RC update on the throttle at least this often - spin down



