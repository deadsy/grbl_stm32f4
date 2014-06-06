
#include <inttypes.h>

#include "serial.h"
#include "config.h"
#include "motion_control.h"
#include "protocol.h"

int serial_rx_hook(uint8_t data)
{
    int rc = 1;

    // Pick off runtime command characters directly from the serial stream. These characters are
    // not passed into the buffer, but these set system state flag bits for runtime execution.
    switch (data) {
        case CMD_STATUS_REPORT: {
            sys.execute |= EXEC_STATUS_REPORT;
            break;
        }
        case CMD_CYCLE_START: {
            sys.execute |= EXEC_CYCLE_START;
            break;
        }
        case CMD_FEED_HOLD: {
            sys.execute |= EXEC_FEED_HOLD;
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
