TARGET = resistance_remastered
OBJS = main.o exports.o

CFLAGS = -Os -G0 -Wall -fshort-wchar -fno-pic -mno-check-zero-division -std=c99
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS)
LDFLAGS = -mno-crt0 -nostartfiles
PSP_FW_VERSION=660

BUILD_PRX = 1
PRX_EXPORTS = exports.exp

USE_KERNEL_LIBS = 1
USE_KERNEL_LIBC = 1

LIBS = -lpspsystemctrl_kernel

PSPSDK = $(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build_prx.mak
