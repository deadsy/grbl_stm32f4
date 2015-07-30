//----------------------------------------------------------------------------
/*

stepper.c - stepper motor driver: executes motion plans using stepper motors

Derived from grbl stepper.c.

* Modified to use STM32F4 timers rather than the AVR timers.
* Direction and step bits are split to allow use on multiple ports.

*/
//----------------------------------------------------------------------------

#include "stepper.h"
#include "config.h"
#include "settings.h"
#include "planner.h"
#include "gpio.h"
#include "timers.h"

//----------------------------------------------------------------------------

#define CYCLES_PER_ACCELERATION_TICK ((TICKS_PER_MICROSECOND*1000000)/ACCELERATION_TICKS_PER_SECOND)

//----------------------------------------------------------------------------
// Stepper state variable. Contains running data and trapezoid variables.

typedef struct {
  // Used by the bresenham line algorithm
  int32_t counter_x,                        // Counter variables for the bresenham line tracer
          counter_y,
          counter_z;
  uint32_t event_count;
  uint32_t step_events_completed;           // The number of step events completed

  // Used by the trapezoid generator
  uint32_t cycles_per_step_event;           // The number of machine cycles between each step event
  uint32_t trapezoid_tick_cycle_counter;    // The cycles since last trapezoid_tick. Used to generate ticks at a steady
                                            // pace without allocating a separate timer
  uint32_t trapezoid_adjusted_rate;         // The current rate of step_events according to the trapezoid generator
  uint32_t min_safe_rate;                   // Minimum safe rate for full deceleration rate reduction step. Otherwise halves step_rate.

} stepper_t;

static stepper_t st;

//----------------------------------------------------------------------------

static block_t *current_block;      // A pointer to the block currently being traced
static uint16_t step_bits;          // step bits
static uint16_t dirn_bits;          // direction bits
static uint16_t saved_step_bits;    // saved step bits

//----------------------------------------------------------------------------

static void set_step_events_per_minute(uint32_t steps_per_minute)
{
    uint32_t ticks;

    if (steps_per_minute < MINIMUM_STEPS_PER_MINUTE) {
        steps_per_minute = MINIMUM_STEPS_PER_MINUTE;
    }

    ticks = (TICKS_PER_MICROSECOND * 1000000 * 60)/steps_per_minute;
    st.cycles_per_step_event = ticks;
    set_step_period(ticks);
}

//----------------------------------------------------------------------------
// This function determines an acceleration velocity change every CYCLES_PER_ACCELERATION_TICK by
// keeping track of the number of elapsed cycles during a de/ac-celeration. The code assumes that
// step_events occur significantly more often than the acceleration velocity iterations.

inline static int iterate_trapezoid_cycle_counter(void)
{
    st.trapezoid_tick_cycle_counter += st.cycles_per_step_event;
    if(st.trapezoid_tick_cycle_counter > CYCLES_PER_ACCELERATION_TICK) {
        st.trapezoid_tick_cycle_counter -= CYCLES_PER_ACCELERATION_TICK;
        return 1;
    } else {
        return 0;
    }
}

//----------------------------------------------------------------------------
// "The Stepper Driver Interrupt" - This timer interrupt is the workhorse of Grbl.
// It is executed at the rate set with set_step_events_per_minute.
// It pops blocks from the block_buffer and executes them by pulsing the stepper pins appropriately.
// It is supported by stepper_pulse_isr() which resets the stepper port after each pulse.
// The bresenham line tracer algorithm controls all three stepper outputs simultaneously with these two interrupts.

void step_period_isr(void)
{
  // Set the direction pins
  dirn_wr(dirn_bits);
#ifdef STEP_PULSE_DELAY
  // save step_bits for use in step_delay_isr()
  saved_step_bits = step_bits;
#else
  // set the step pins
  step_wr(step_bits);
#endif

  // If there is no current block, attempt to pop one from the buffer
  if (current_block == NULL) {
    // Anything in the buffer? If so, initialize next motion.
    current_block = plan_get_current_block();
    if (current_block != NULL) {
      if (sys.state == STATE_CYCLE) {
        // During feed hold, do not update rate and trap counter. Keep decelerating.
        st.trapezoid_adjusted_rate = current_block->initial_rate;
        set_step_events_per_minute(st.trapezoid_adjusted_rate); // Initialize cycles_per_step_event
        st.trapezoid_tick_cycle_counter = CYCLES_PER_ACCELERATION_TICK/2; // Start halfway for midpoint rule.
      }
      st.min_safe_rate = current_block->rate_delta + (current_block->rate_delta >> 1); // 1.5 x rate_delta
      st.counter_x = -(current_block->step_event_count >> 1);
      st.counter_y = st.counter_x;
      st.counter_z = st.counter_x;
      st.event_count = current_block->step_event_count;
      st.step_events_completed = 0;
    } else {
      st_go_idle();
      bit_true(sys.execute,EXEC_CYCLE_STOP); // Flag main program for cycle end
    }
  }

  if (current_block != NULL) {
    // Execute step displacement profile by bresenham line algorithm
    dirn_bits = current_block->direction_bits;
    step_bits = 0;
    st.counter_x += current_block->steps_x;
    if (st.counter_x > 0) {
      step_bits |= (1<<X_STEP_BIT);
      st.counter_x -= st.event_count;
      if (dirn_bits & (1<<X_DIRECTION_BIT)) { sys.position[X_AXIS]--; }
      else { sys.position[X_AXIS]++; }
    }
    st.counter_y += current_block->steps_y;
    if (st.counter_y > 0) {
#if defined(AXES_XYZY)
      step_bits |= ((1 << A_STEP_BIT) | (1 << Y_STEP_BIT));
#else
      step_bits |= (1 << Y_STEP_BIT);
#endif
      st.counter_y -= st.event_count;
      if (dirn_bits & (1<<Y_DIRECTION_BIT)) { sys.position[Y_AXIS]--; }
      else { sys.position[Y_AXIS]++; }
    }
    st.counter_z += current_block->steps_z;
    if (st.counter_z > 0) {
      step_bits |= (1<<Z_STEP_BIT);
      st.counter_z -= st.event_count;
      if (dirn_bits & (1<<Z_DIRECTION_BIT)) { sys.position[Z_AXIS]--; }
      else { sys.position[Z_AXIS]++; }
    }

    st.step_events_completed ++; // Iterate step events

    // While in block steps, check for de/ac-celeration events and execute them accordingly.
    if (st.step_events_completed < current_block->step_event_count) {
      if (sys.state == STATE_HOLD) {
        // Check for and execute feed hold by enforcing a steady deceleration from the moment of
        // execution. The rate of deceleration is limited by rate_delta and will never decelerate
        // faster or slower than in normal operation. If the distance required for the feed hold
        // deceleration spans more than one block, the initial rate of the following blocks are not
        // updated and deceleration is continued according to their corresponding rate_delta.
        // NOTE: The trapezoid tick cycle counter is not updated intentionally. This ensures that
        // the deceleration is smooth regardless of where the feed hold is initiated and if the
        // deceleration distance spans multiple blocks.
        if ( iterate_trapezoid_cycle_counter() ) {
          // If deceleration complete, set system flags and shutdown steppers.
          if (st.trapezoid_adjusted_rate <= current_block->rate_delta) {
            // Just go idle. Do not NULL current block. The bresenham algorithm variables must
            // remain intact to ensure the stepper path is exactly the same. Feed hold is still
            // active and is released after the buffer has been reinitialized.
            st_go_idle();
            bit_true(sys.execute,EXEC_CYCLE_STOP); // Flag main program that feed hold is complete.
          } else {
            st.trapezoid_adjusted_rate -= current_block->rate_delta;
            set_step_events_per_minute(st.trapezoid_adjusted_rate);
          }
        }

      } else {
        // The trapezoid generator always checks step event location to ensure de/ac-celerations are
        // executed and terminated at exactly the right time. This helps prevent over/under-shooting
        // the target position and speed.
        // NOTE: By increasing the ACCELERATION_TICKS_PER_SECOND in config.h, the resolution of the
        // discrete velocity changes increase and accuracy can increase as well to a point. Numerical
        // round-off errors can effect this, if set too high. This is important to note if a user has
        // very high acceleration and/or feedrate requirements for their machine.
        if (st.step_events_completed < current_block->accelerate_until) {
          // Iterate cycle counter and check if speeds need to be increased.
          if ( iterate_trapezoid_cycle_counter() ) {
            st.trapezoid_adjusted_rate += current_block->rate_delta;
            if (st.trapezoid_adjusted_rate >= current_block->nominal_rate) {
              // Reached nominal rate a little early. Cruise at nominal rate until decelerate_after.
              st.trapezoid_adjusted_rate = current_block->nominal_rate;
            }
            set_step_events_per_minute(st.trapezoid_adjusted_rate);
          }
        } else if (st.step_events_completed >= current_block->decelerate_after) {
          // Reset trapezoid tick cycle counter to make sure that the deceleration is performed the
          // same every time. Reset to CYCLES_PER_ACCELERATION_TICK/2 to follow the midpoint rule for
          // an accurate approximation of the deceleration curve. For triangle profiles, down count
          // from current cycle counter to ensure exact deceleration curve.
          if (st.step_events_completed == current_block-> decelerate_after) {
            if (st.trapezoid_adjusted_rate == current_block->nominal_rate) {
              st.trapezoid_tick_cycle_counter = CYCLES_PER_ACCELERATION_TICK/2; // Trapezoid profile
            } else {
              st.trapezoid_tick_cycle_counter = CYCLES_PER_ACCELERATION_TICK-st.trapezoid_tick_cycle_counter; // Triangle profile
            }
          } else {
            // Iterate cycle counter and check if speeds need to be reduced.
            if ( iterate_trapezoid_cycle_counter() ) {
              // NOTE: We will only do a full speed reduction if the result is more than the minimum safe
              // rate, initialized in trapezoid reset as 1.5 x rate_delta. Otherwise, reduce the speed by
              // half increments until finished. The half increments are guaranteed not to exceed the
              // CNC acceleration limits, because they will never be greater than rate_delta. This catches
              // small errors that might leave steps hanging after the last trapezoid tick or a very slow
              // step rate at the end of a full stop deceleration in certain situations. The half rate
              // reductions should only be called once or twice per block and create a nice smooth
              // end deceleration.
              if (st.trapezoid_adjusted_rate > st.min_safe_rate) {
                st.trapezoid_adjusted_rate -= current_block->rate_delta;
              } else {
                st.trapezoid_adjusted_rate >>= 1; // Bit shift divide by 2
              }
              if (st.trapezoid_adjusted_rate < current_block->final_rate) {
                // Reached final rate a little early. Cruise to end of block at final rate.
                st.trapezoid_adjusted_rate = current_block->final_rate;
              }
              set_step_events_per_minute(st.trapezoid_adjusted_rate);
            }
          }
        } else {
          // No accelerations. Make sure we cruise exactly at the nominal rate.
          if (st.trapezoid_adjusted_rate != current_block->nominal_rate) {
            st.trapezoid_adjusted_rate = current_block->nominal_rate;
            set_step_events_per_minute(st.trapezoid_adjusted_rate);
          }
        }
      }
    } else {
      // If current block is finished, reset pointer
      step_bits = 0;
      current_block = NULL;
      plan_discard_current_block();
    }
  }
  step_bits ^= settings.step_invert_mask;
  dirn_bits ^= settings.dirn_invert_mask;
}

//----------------------------------------------------------------------------
// This ISR is called at the end of the pulse period. We have generated a
// pulse of sufficient width, so now we can set the step line low.

void step_pulse_isr(void)
{
    // Reset stepping pins
    step_wr(settings.step_invert_mask);
}

//----------------------------------------------------------------------------
// This ISR is called at the end of the direction to pulse delay period.
// We (possibly) altered the direction pin at the start of the period. Now we
// have delayed a short time and can start the step pulse.

void step_delay_isr(void)
{
    // Begin step pulse.
    step_wr(saved_step_bits);
}

//----------------------------------------------------------------------------
// Stepper state initialization. Cycle should only start if the st.cycle_start flag is
// enabled. Startup init and limits call this function but shouldn't start the cycle.

void st_wake_up(void)
{
    // Enable the stepper motors
    stepper_motor_enable();

    if (sys.state == STATE_CYCLE) {
        // Initialize stepper output bits
        step_bits = settings.step_invert_mask;
        dirn_bits = settings.dirn_invert_mask;
        // Initialize step pulse timing from settings. Here to ensure updating after re-writing.
#ifdef STEP_PULSE_DELAY
        set_step_pulse_delay(STEP_PULSE_DELAY * TICKS_PER_MICROSECOND);
        set_step_pulse_time((settings.pulse_microseconds + STEP_PULSE_DELAY) * TICKS_PER_MICROSECOND);
#else // Normal operation
        set_step_pulse_delay(0);
        set_step_pulse_time(settings.pulse_microseconds * TICKS_PER_MICROSECOND);
#endif
        // Enable stepper driver interrupt
        step_isr_enable();
    }
}

//----------------------------------------------------------------------------
// Stepper shutdown

void st_go_idle(void)
{
    // Disable stepper driver interrupt
    step_isr_disable();
    // Disable steppers only upon system alarm activated or by user setting to not be kept enabled.
    if ((settings.stepper_idle_lock_time != 0xff) || bit_istrue(sys.execute,EXEC_ALARM)) {
        // Force stepper dwell to lock axes for a defined amount of time to ensure the axes come to a complete
        // stop and not drift from residual inertial forces at the end of the last movement.
#warning "TODO: the systick needs to run at a higher irq priority than tim2, else delay_ms turns into an infinite loop"
        //delay_ms(settings.stepper_idle_lock_time);
        stepper_motor_disable();
    }
}

//----------------------------------------------------------------------------
// Reset and clear stepper subsystem variables

void st_reset(void)
{
    memset(&st, 0, sizeof(st));
    set_step_events_per_minute(MINIMUM_STEPS_PER_MINUTE);
    current_block = NULL;
}

//----------------------------------------------------------------------------
// Initialize and setup the stepper motor subsystem

void st_init(void)
{
    step_wr(settings.step_invert_mask);
    // Start in the idle state, but first wake up to check for keep steppers enabled option.
    st_wake_up();
    st_go_idle();
}

//----------------------------------------------------------------------------
// Planner external interface to start stepper interrupt and execute the blocks in queue. Called
// by the main program functions: planner auto-start and run-time command execution.

void st_cycle_start(void)
{
    if (sys.state == STATE_QUEUED) {
        sys.state = STATE_CYCLE;
        st_wake_up();
    }
}

//----------------------------------------------------------------------------
// Execute a feed hold with deceleration, only during cycle. Called by main program.

void st_feed_hold(void)
{
    if (sys.state == STATE_CYCLE) {
        sys.state = STATE_HOLD;
        sys.auto_start = false; // Disable planner auto start upon feed hold.
    }
}

//----------------------------------------------------------------------------
// Reinitializes the cycle plan and stepper system after a feed hold for a resume. Called by
// runtime command execution in the main program, ensuring that the planner re-plans safely.
// NOTE: Bresenham algorithm variables are still maintained through both the planner and stepper
// cycle reinitializations. The stepper path should continue exactly as if nothing has happened.
// Only the planner de/ac-celerations profiles and stepper rates have been updated.

void st_cycle_reinitialize(void)
{
    if (current_block != NULL) {
        // Replan buffer from the feed hold stop location.
        plan_cycle_reinitialize(current_block->step_event_count - st.step_events_completed);
        // Update initial rate and timers after feed hold.
        st.trapezoid_adjusted_rate = 0; // Resumes from rest
        set_step_events_per_minute(st.trapezoid_adjusted_rate);
        st.trapezoid_tick_cycle_counter = CYCLES_PER_ACCELERATION_TICK/2; // Start halfway for midpoint rule.
        st.step_events_completed = 0;
        sys.state = STATE_QUEUED;
    } else {
        sys.state = STATE_IDLE;
    }
}
//----------------------------------------------------------------------------
