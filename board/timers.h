//-----------------------------------------------------------------------------
/*

Timer Functions

The main stepper routines use timers and ISRs for pulse generation.
This file has the board specific routines to allow this.

*/
//-----------------------------------------------------------------------------

#ifndef TIMERS_H
#define TIMERS_H

//-----------------------------------------------------------------------------

#define TICKS_PER_MICROSECOND 8

//-----------------------------------------------------------------------------

void stepper_isr_enable(void);
void stepper_isr_disable(void);
void steppers_enable(void);
void steppers_disable(void);

void charge_pump_stop(void);
void timers_init(void);

//-----------------------------------------------------------------------------

#endif // TIMERS_H

//-----------------------------------------------------------------------------