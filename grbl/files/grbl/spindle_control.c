//----------------------------------------------------------------------------
/*

spindle control

*/
//----------------------------------------------------------------------------

#include "grbl.h"

//----------------------------------------------------------------------------

void spindle_init(void) {
  spindle_stop();
}

uint8_t spindle_get_state(void) {
  return SPINDLE_STATE_DISABLE;
}

void spindle_stop(void) {
}

#ifdef VARIABLE_SPINDLE

void spindle_set_speed(uint8_t pwm_value) {
}

uint8_t spindle_compute_pwm_value(float rpm) {
  return 0;
}

void spindle_set_state(uint8_t state, float rpm) {
}

void spindle_sync(uint8_t state, float rpm) {
}

#else

void _spindle_set_state(uint8_t state) {
}

void _spindle_sync(uint8_t state) {
}

#endif // not VARIABLE_SPINDLE

//----------------------------------------------------------------------------
