
#include <inttypes.h>

#include "serial.h"
#include "config.h"
#include "motion_control.h"
#include "protocol.h"

int serial_rx_hook(uint8_t data)
{
  int rc = 1;

  // Pick off realtime command characters directly from the serial stream. These characters are
  // not passed into the buffer, but these set system state flag bits for realtime execution.
  switch (data) {
    case CMD_STATUS_REPORT: {
      bit_true_atomic(sys.rt_exec_state, EXEC_STATUS_REPORT);
      break;
    }
    case CMD_CYCLE_START: {
      bit_true_atomic(sys.rt_exec_state, EXEC_CYCLE_START);
      break;
    }
    case CMD_FEED_HOLD: {
      bit_true_atomic(sys.rt_exec_state, EXEC_FEED_HOLD);
      break;
    }
    case CMD_SAFETY_DOOR: {
      bit_true_atomic(sys.rt_exec_state, EXEC_SAFETY_DOOR);
      break;
    }
    case CMD_RESET: {
      mc_reset();
      break;
    }
    default: {
      rc = 0;
      break;
    }
  }
  return rc;
}
