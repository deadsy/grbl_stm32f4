TOP = ../..
include $(TOP)/mk/common.mk

SRC = coolant_control.c \
      delay.c \
      eeprom.c \
      gcode.c \
      limits.c \
      main.c \
      motion_control.c \
      nuts_bolts.c \
      planner.c \
      print.c \
      probe.c \
      protocol.c \
      report.c \
      settings.c \
      spindle_control.c \
      serial.c \
      stepper.c \
      system.c \

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
	$(X_GCC) $(INCLUDE) $(DEFINE) $(X_CFLAGS) -c $*.c

libgrbl.a: $(OBJ)
	$(X_AR) rcs $@ $(OBJ)

clean:
	rm -f *.o *.a
