TOP = ../..
include $(TOP)/mk/common.mk

SRC = grbl/coolant_control.c \
      grbl/delay.c \
      grbl/eeprom.c \
      grbl/gcode.c \
      grbl/jog.c \
      grbl/limits.c \
      grbl/main.c \
      grbl/motion_control.c \
      grbl/nuts_bolts.c \
      grbl/planner.c \
      grbl/print.c \
      grbl/probe.c \
      grbl/protocol.c \
      grbl/report.c \
      grbl/settings.c \
      grbl/spindle_control.c \
      grbl/serial.c \
      grbl/stepper.c \
      grbl/system.c \

OBJ = $(SRC:.c=.o)

# include paths
INCLUDE += -I$(TOP)/board/st/stm32f4/mb997
INCLUDE += -I$(TOP)/board/st/stm32f4/hal/inc
INCLUDE += -I$(TOP)/board/st/stm32f4/cmsis

# defines
DEFINE += -DENABLE_M7
DEFINE += -DSTM32F407xx

# turn off some warnings
X_CFLAGS += -Wno-strict-prototypes
X_CFLAGS += -Wno-unused-parameter

.c.o:
	$(X_GCC) $(INCLUDE) $(DEFINE) $(X_CFLAGS) -c $< -o $@

libgrbl.a: $(OBJ)
	$(X_AR) rcs $@ $(OBJ)

clean:
	rm -f *.o *.a
