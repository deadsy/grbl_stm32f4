TOP = ../..

GRBL = grbl-master
WORK_DIR = $(GRBL)
TARFILE = $(GRBL).tar.bz2

SRC = coolant_control.c \
      gcode.c \
      limits.c \
      main.c \
      motion_control.c \
      nuts_bolts.c \
      planner.c \
      print.c \
      protocol.c \
      report.c \
      settings.c \
      spindle_control.c \
      stepper.c \

INC = .

OBJ = $(patsubst %.c, $(WORK_DIR)/%.o, $(SRC))

all: untar $(OBJ)

clean:
	-rm -rf $(WORK_DIR)

include $(TOP)/mk/common.mk
