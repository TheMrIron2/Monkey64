TARGET = Monkey64
PSPPORT = \
    Video_PSP.o \
    ROM_PSP.o \
    Helper_PSP.o \
    Menu_PSP.o \
    $(CORE) \
    $(CPU) \
    $(HLEUC)

CPU = \
    CPU/Memory.o \
    CPU/Registers.o \
    CPU/Timers.o \
    CPU/Exceptions.o \
    CPU/DMA.o \
    CPU/Video.o \
    CPU/PIF.o \
    CPU/Textures.o

CORE = \
    CPU/Instructions/Core.o \
    CPU/Instructions/COP0.o \
    CPU/Instructions/COP1.o \
    CPU/Instructions/RSP.o \
    CPU/Instructions/RDPLLE.o

HLEUC = \
    CPU/Instructions/RSPFast3D.o \
    CPU/Instructions/RSPF3DEX.o \
    CPU/Instructions/RSPF3DEX2.o

OBJS = main.o $(PSPPORT)

INCDIR =
CFLAGS = -O2 -G0 -Wall
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS)

LIBDIR =
LDFLAGS =
LIBS= -lPSPgum_vfpu -lPSPvfpu -lPSPgu -lm -lPSPpower

EXTRA_TARGETS = EBOOT.PBP
PSP_EBOOT_TITLE = Monkey 64
PSP_FW_VERSION = 500
BUILD_PRX = 1
PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak
