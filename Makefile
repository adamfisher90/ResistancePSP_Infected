TARGET = resistance_remastered
OBJS = main.o exports.o

INCDIR = -I $(ARKROOT)/common/include
CFLAGS = -Os -G0 -Wall -fshort-wchar -fno-pic -mno-check-zero-division -std=c99
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS)
LDFLAGS = -nostartfiles
PSP_FW_VERSION=660

BUILD_PRX = 1
PRX_EXPORTS = exports.exp

USE_KERNEL_LIBS = 1
USE_KERNEL_LIBC = 1

LIBDIR = $(ARKROOT)/libs
LIBS = -lpspsystemctrl_kernel

all:
	psp-packer $(TARGET).prx

PSPSDK = $(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build_prx.mak

clean:
	$(Q)rm -f *.bin *.elf *.prx *.o