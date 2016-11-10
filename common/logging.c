//-----------------------------------------------------------------------------
/*

Logging Functions

*/
//-----------------------------------------------------------------------------

#include <SEGGER_RTT_Conf.h>
#include <SEGGER_RTT.h>

#include "logging.h"

//-----------------------------------------------------------------------------

#define LOG_TERMINAL_NORMAL 0
#define LOG_TERMINAL_ERROR 1
#define LOG_TERMINAL_INPUT 0

static char buf_normal_up[BUFFER_SIZE_UP];
static char buf_down[BUFFER_SIZE_DOWN];

//-----------------------------------------------------------------------------

int log_init(void) {
  if (SEGGER_RTT_ConfigUpBuffer(LOG_TERMINAL_NORMAL,
                                "Normal",
                                buf_normal_up,
                                BUFFER_SIZE_UP,
                                SEGGER_RTT_MODE_NO_BLOCK_TRIM
                               ) != 0) {
    return -1;
  }
  if (SEGGER_RTT_ConfigDownBuffer(LOG_TERMINAL_INPUT,
                                  "Input",
                                  buf_down,
                                  BUFFER_SIZE_DOWN,
                                  SEGGER_RTT_MODE_NO_BLOCK_SKIP
                                 ) != 0) {
    return -1;
  }
  return 0;
}

//-----------------------------------------------------------------------------

int SEGGER_RTT_vprintf(unsigned BufferIndex, const char *sFormat, va_list *pParamList);

void log_printf(char *format_msg, ...) {
  va_list p_args;
  va_start(p_args, format_msg);
  (void)SEGGER_RTT_vprintf(LOG_TERMINAL_NORMAL, format_msg, &p_args);
  va_end(p_args);
  SEGGER_RTT_WriteString(LOG_TERMINAL_NORMAL, "\r\n");
}

//-----------------------------------------------------------------------------
