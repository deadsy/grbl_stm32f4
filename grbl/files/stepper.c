//----------------------------------------------------------------------------
/*

stepper.c - stepper motor driver: executes motion plans using stepper motors

Derived from grbl stepper.c.

* Modified to use STM32F4 timers rather than the AVR timers.
* Direction and step bits are split to allow use on multiple ports.

*/
//----------------------------------------------------------------------------

#include "grbl.h"

//----------------------------------------------------------------------------

void step_period_isr(void) {}

//----------------------------------------------------------------------------
// This ISR is called at the end of the direction to pulse delay period.
// We (possibly) altered the direction pin at the start of the period. Now we
// have delayed a short time and can start the step pulse.

void step_delay_isr(void) {}

//----------------------------------------------------------------------------
// This ISR is called at the end of the pulse period. We have generated a
// pulse of sufficient width, so now we can set the step line low.

void step_pulse_isr(void) {}

//----------------------------------------------------------------------------

void st_wake_up(void) {}
void st_go_idle(void) {}
void st_generate_step_dir_invert_masks(void) {}
void st_reset(void) {}
void stepper_init(void) {}
void st_update_plan_block_parameters(void) {}
void st_prep_buffer(void) {}

//----------------------------------------------------------------------------
// Called by realtime status reporting to fetch the current speed being executed. This value
// however is not exactly the current speed, but the speed computed in the last step segment
// in the segment buffer. It will always be behind by up to the number of segment blocks (-1)
// divided by the ACCELERATION TICKS PER SECOND in seconds.

#ifdef REPORT_REALTIME_RATE
float st_get_realtime_rate(void)
{
  if (sys.state & (STATE_CYCLE | STATE_HOMING | STATE_HOLD | STATE_MOTION_CANCEL | STATE_SAFETY_DOOR)){
    return prep.current_speed;
  }
  return 0.0f;
}
#endif

//----------------------------------------------------------------------------
