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
// step timing control

// The highest timer rate is (168/2) = 84MHz
// We divide it by 4 to give a counter rate of 21 MHz.
// ie 1 us == 21
#define TICKS_PER_MICROSECOND 21

void stepper_isr_enable(void);
void stepper_isr_disable(void);

void set_step_period(uint32_t ticks);
void set_step_pulse_delay(uint32_t ticks);
void set_step_pulse_time(uint32_t ticks);

// interrupt context callbacks to generate step/direction pulses
void step_period_isr(void);
void step_pulse_isr(void);
void step_delay_isr(void);

//-----------------------------------------------------------------------------
// g540 charge pump

void charge_pump_stop(void);

//-----------------------------------------------------------------------------

void timers_init(void);

//-----------------------------------------------------------------------------

#endif // TIMERS_H

//-----------------------------------------------------------------------------