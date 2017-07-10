
#include "grbl.h"

int serial_rx_hook(uint8_t data)
{
  int rc = 1;

  // Pick off realtime command characters directly from the serial stream. These characters are
  // not passed into the main buffer, but these set system state flag bits for realtime execution.
  switch (data) {
    case CMD_RESET: {
      mc_reset();
      break;
    }
    case CMD_STATUS_REPORT: {
      system_set_exec_state_flag(EXEC_STATUS_REPORT);
      break;
    }
    case CMD_CYCLE_START: {
      system_set_exec_state_flag(EXEC_CYCLE_START);
      break;
    }
    case CMD_FEED_HOLD: {
      system_set_exec_state_flag(EXEC_FEED_HOLD);
      break;
    }
    default: {
      // Real-time control characters are extended ACSII only.
      if (data > 0x7F) {
        switch(data) {
          case CMD_SAFETY_DOOR: {
            system_set_exec_state_flag(EXEC_SAFETY_DOOR);
            break;
          }
          case CMD_JOG_CANCEL: {
            if (sys.state & STATE_JOG) { // Block all other states from invoking motion cancel.
              system_set_exec_state_flag(EXEC_MOTION_CANCEL);
            }
            break;
          }
          #ifdef DEBUG
          case CMD_DEBUG_REPORT: {
            uint8_t sreg = SREG; cli(); bit_true(sys_rt_exec_debug,EXEC_DEBUG_REPORT); SREG = sreg;
            break;
          }
          #endif
          case CMD_FEED_OVR_RESET: {
            system_set_exec_motion_override_flag(EXEC_FEED_OVR_RESET);
            break;
          }
          case CMD_FEED_OVR_COARSE_PLUS: {
            system_set_exec_motion_override_flag(EXEC_FEED_OVR_COARSE_PLUS);
            break;
          }
          case CMD_FEED_OVR_COARSE_MINUS: {
            system_set_exec_motion_override_flag(EXEC_FEED_OVR_COARSE_MINUS);
            break;
          }
          case CMD_FEED_OVR_FINE_PLUS: {
            system_set_exec_motion_override_flag(EXEC_FEED_OVR_FINE_PLUS);
            break;
          }
          case CMD_FEED_OVR_FINE_MINUS: {
            system_set_exec_motion_override_flag(EXEC_FEED_OVR_FINE_MINUS);
            break;
          }
          case CMD_RAPID_OVR_RESET: {
            system_set_exec_motion_override_flag(EXEC_RAPID_OVR_RESET);
            break;
          }
          case CMD_RAPID_OVR_MEDIUM: {
            system_set_exec_motion_override_flag(EXEC_RAPID_OVR_MEDIUM);
            break;
          }
          case CMD_RAPID_OVR_LOW: {
            system_set_exec_motion_override_flag(EXEC_RAPID_OVR_LOW);
            break;
          }
          case CMD_SPINDLE_OVR_RESET: {
            system_set_exec_accessory_override_flag(EXEC_SPINDLE_OVR_RESET);
            break;
          }
          case CMD_SPINDLE_OVR_COARSE_PLUS: {
            system_set_exec_accessory_override_flag(EXEC_SPINDLE_OVR_COARSE_PLUS);
            break;
          }
          case CMD_SPINDLE_OVR_COARSE_MINUS: {
            system_set_exec_accessory_override_flag(EXEC_SPINDLE_OVR_COARSE_MINUS);
            break;
          }
          case CMD_SPINDLE_OVR_FINE_PLUS: {
            system_set_exec_accessory_override_flag(EXEC_SPINDLE_OVR_FINE_PLUS);
            break;
          }
          case CMD_SPINDLE_OVR_FINE_MINUS: {
            system_set_exec_accessory_override_flag(EXEC_SPINDLE_OVR_FINE_MINUS);
            break;
          }
          case CMD_SPINDLE_OVR_STOP: {
            system_set_exec_accessory_override_flag(EXEC_SPINDLE_OVR_STOP);
            break;
          }
          case CMD_COOLANT_FLOOD_OVR_TOGGLE: {
            system_set_exec_accessory_override_flag(EXEC_COOLANT_FLOOD_OVR_TOGGLE);
            break;
          }
          #ifdef ENABLE_M7
          case CMD_COOLANT_MIST_OVR_TOGGLE: {
            system_set_exec_accessory_override_flag(EXEC_COOLANT_MIST_OVR_TOGGLE);
            break;
          }
          #endif
        }
      } else {
        rc = 0;
      }
    }
  }
  return rc;
}
