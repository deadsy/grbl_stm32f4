
# host compilation tools
HOST_GCC = gcc

# cross compilation tools
XTOOLS_DIR = /opt/gcc-arm-none-eabi-5_4-2016q2
X_LIBC_DIR = $(XTOOLS_DIR)/arm-none-eabi/lib/armv7e-m/fpu
X_LIBGCC_DIR = $(XTOOLS_DIR)/lib/gcc/arm-none-eabi/5.4.1/armv7e-m/fpu
X_GCC = $(XTOOLS_DIR)/bin/arm-none-eabi-gcc
X_OBJCOPY = $(XTOOLS_DIR)/bin/arm-none-eabi-objcopy
X_AR = $(XTOOLS_DIR)/bin/arm-none-eabi-ar
X_LD = $(XTOOLS_DIR)/bin/arm-none-eabi-ld
X_GDB = $(XTOOLS_DIR)/bin/arm-none-eabi-gdb

# cross compile flags for cortex m4 build
X_CFLAGS = -Werror -Wall -Wextra -Wstrict-prototypes
X_CFLAGS += -O2
X_CFLAGS += -falign-functions -fomit-frame-pointer -fno-strict-aliasing
X_CFLAGS += -mlittle-endian -mthumb -mcpu=cortex-m4 -mthumb-interwork
X_CFLAGS += -mfloat-abi=hard -mfpu=fpv4-sp-d16
