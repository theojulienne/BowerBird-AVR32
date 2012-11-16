# Copyright (C) 2009 - 2010 Atmel Corporation. All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
# 1. Redistributions of source code must retain the above copyright notice,
# this list of conditions and the following disclaimer.
#
# 2. Redistributions in binary form must reproduce the above copyright notice,
# this list of conditions and the following disclaimer in the documentation
# and/or other materials provided with the distribution.
#
# 3. The name of Atmel may not be used to endorse or promote products derived
# from this software without specific prior written permission.
#
# 4. This software may only be redistributed and used in connection with an
# Atmel AVR product.
#
# THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
# WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
# EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
# ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
# SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
# CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
# OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

# Path to top level ASF directory relative to this project directory.
PRJ_PATH = /Users/theo/Development/avr32-install/asf-2.8.1

# Target CPU architecture: ap, ucr1, ucr2 or ucr3
ARCH = ucr2

# Target part: none, ap7xxx or uc3xxxxx
PART = uc3a3256

# Target device flash memory details (used by the avr32program programming
# tool: [cfi|internal]@address
FLASH = internal@0x80000000

# Clock source to use when programming; xtal, extclk or int
PROG_CLOCK = xtal

# Application target name. Given with suffix .a for library and .elf for a
# standalone application.
TARGET = powerBoard.elf

# List of C source files.
CSRCS = \
       main.c \
       comms.c \
       init.c \
       lcd/lcd.c \
       avr32/drivers/pm/power_clocks_lib.c                \
       avr32/drivers/usart/usart.c                        \
       common/services/usb/class/cdc/device/udi_cdc.c     \
       common/services/sleepmgr/uc3/sleepmgr.c            \
       common/utils/stdio/read.c                          \
       common/services/usb/udc/udc.c                      \
       avr32/drivers/intc/intc.c                          \
       common/services/usb/class/cdc/device/udi_cdc_desc.c \
       common/utils/stdio/write.c                         \
       avr32/drivers/pm/pm.c                              \
       avr32/drivers/pm/pm_conf_clocks.c                  \
       avr32/drivers/gpio/gpio.c                          \
       common/services/clock/uc3a3_a4/sysclk.c            \
       avr32/drivers/usbb/usbb_device.c                   \
       avr32/drivers/flashc/flashc.c                      \
       common/utils/stdio/stdio_usb/stdio_usb.c           \
       common/services/fifo/fifo.c                        \
       avr32/drivers/wdt/wdt.c

# List of assembler source files.
ASSRCS = \
       avr32/utils/startup/startup_uc3.S                  \
       avr32/utils/startup/trampoline_uc3.S               \
       avr32/drivers/intc/exception.S

# List of include paths.
INC_PATH = \
       common/services/usb/class/cdc                      \
       common/services/usb/class/cdc/device               \
       common/services/sleepmgr                           \
       avr32/drivers/usart							      \
       avr32/drivers/gpio                                 \
       common/services/clock                              \
       avr32/drivers/usbb                                 \
       common/boards                                      \
       common/utils/stdio/stdio_usb                       \
       avr32/drivers/flashc                               \
       common/services/usb/udc                            \
       avr32/boards                                       \
       avr32/utils                                        \
       common/utils/stdio/stdio_usb/stdio_usb_example/at32uc3a3256_uc3_a3_xplained \
       avr32/utils/preprocessor                           \
       common/utils                                       \
       avr32/drivers/pm                                   \
       avr32/drivers/intc                                 \
       common/services/usb                                \
       common/services/fifo                               \
       avr32/drivers/wdt                                  \
       avr32/boards/uc3_a3_xplained \
       ./common/utils/stdio/stdio_usb/stdio_usb_example/at32uc3a3256_uc3_a3_xplained/gcc

# Additional search paths for libraries.
LIB_PATH = 

# List of libraries to use during linking.
LIBS = 

# Path relative to top level directory pointing to a linker script.
LINKER_SCRIPT = avr32/utils/linker_scripts/at32uc3a3/256/gcc/link_uc3a3256.lds

# Additional options for debugging. By default the common Makefile.in will
# add -g3.
DBGFLAGS = 

# Application optimization used during compilation and linking:
# -O0, -O1, -O2, -O3 or -Os
OPTIMIZATION = -Os

# Extra flags to use when archiving.
ARFLAGS = 

# Extra flags to use when assembling.
ASFLAGS = 

# Extra flags to use when compiling.
CFLAGS = 

# Extra flags to use when preprocessing.
#
# Preprocessor symbol definitions
#   To add a definition use the format "-D name[=definition]".
#   To cancel a definition use the format "-U name".
#
# The most relevant symbols to define for the preprocessor are:
#   BOARD      Target board in use, see boards/board.h for a list.
#   EXT_BOARD  Optional extension board in use, see boards/board.h for a list.
CPPFLAGS = \
       -D BOARD=USER_BOARD -I.

# Extra flags to use when linking
LDFLAGS = \
       -nostartfiles -Wl,-e,_trampoline
