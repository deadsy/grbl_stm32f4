#ifndef defaults_h
#define defaults_h

// Description:
// KRMX01 3-Axis CNC Router with 0.5"/rev lead screws
// Gecko G540 4-Axis Stepper Controller
// 320 Oz-Inch, 200 steps/rev Motors

#define MICROSTEPS 10.0
#define STEPS_PER_REV 200.0
#define MM_PER_REV (0.5 * MM_PER_INCH) // 0.5 inch/rev leadscrew

#define DEFAULT_X_STEPS_PER_MM (STEPS_PER_REV*MICROSTEPS/MM_PER_REV)
#define DEFAULT_Y_STEPS_PER_MM (STEPS_PER_REV*MICROSTEPS/MM_PER_REV)
#define DEFAULT_Z_STEPS_PER_MM (STEPS_PER_REV*MICROSTEPS/MM_PER_REV)
#define DEFAULT_X_MAX_RATE 500.0 // mm/min
#define DEFAULT_Y_MAX_RATE 500.0 // mm/min
#define DEFAULT_Z_MAX_RATE 500.0 // mm/min
#define DEFAULT_X_ACCELERATION (10.0*60*60) // 10*60*60 mm/min^2 = 10 mm/sec^2
#define DEFAULT_Y_ACCELERATION (10.0*60*60) // 10*60*60 mm/min^2 = 10 mm/sec^2
#define DEFAULT_Z_ACCELERATION (10.0*60*60) // 10*60*60 mm/min^2 = 10 mm/sec^2
#define DEFAULT_X_MAX_TRAVEL 200.0 // mm
#define DEFAULT_Y_MAX_TRAVEL 200.0 // mm
#define DEFAULT_Z_MAX_TRAVEL 200.0 // mm
#define DEFAULT_SPINDLE_RPM_MAX 1000.0 // rpm
#define DEFAULT_SPINDLE_RPM_MIN 0.0 // rpm
#define DEFAULT_STEP_PULSE_MICROSECONDS 10
#define DEFAULT_STEPPING_INVERT_MASK 0
#define DEFAULT_DIRECTION_INVERT_MASK ((1 << Z_DIRECTION_BIT) | (1 << Y_DIRECTION_BIT) | (1 << A_DIRECTION_BIT))
#define DEFAULT_STEPPER_IDLE_LOCK_TIME 25 // msec (0-255)
#define DEFAULT_STATUS_REPORT_MASK 1 // MPos enabled
#define DEFAULT_JUNCTION_DEVIATION 0.01 // mm
#define DEFAULT_ARC_TOLERANCE 0.002 // mm
#define DEFAULT_REPORT_INCHES 0 // false
#define DEFAULT_INVERT_ST_ENABLE 0 // false
#define DEFAULT_INVERT_LIMIT_PINS 0 // false
#define DEFAULT_SOFT_LIMIT_ENABLE 0 // false
#define DEFAULT_HARD_LIMIT_ENABLE 0  // false
#define DEFAULT_INVERT_PROBE_PIN 0 // false
#define DEFAULT_LASER_MODE 0 // false
#define DEFAULT_HOMING_ENABLE 1  // true
#define DEFAULT_HOMING_DIR_MASK 0 // move positive dir
#define DEFAULT_HOMING_FEED_RATE 25.0 // mm/min
#define DEFAULT_HOMING_SEEK_RATE 500.0 // mm/min
#define DEFAULT_HOMING_DEBOUNCE_DELAY 100 // msec (0-65k)
#define DEFAULT_HOMING_PULLOFF 1.0 // mm

#endif












