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

CORE_SRCS = $(PSPPORT) $(CORE) $(CPU) $(HLEUC)
SRCS			= $(CORE_SRCS)

INCDIR   =
CFLAGS   = -O1 -G0 -Wall
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS  = $(CFLAGS)

OBJS := $(SRCS:.cpp=.o)
OBJS := $(OBJS:.c=.o)
OBJS := $(OBJS:.S=.o)

LIBDIR  =
LDFLAGS =
LIBS    = -lpspgum_vfpu -lpspvfpu -lpspgu -lm -lpsppower

EXTRA_TARGETS    = EBOOT.PBP
PSP_EBOOT_TITLE  = Monkey 64
PSP_FW_VERSION   = 500
PSP_EBOOT_ICON   = icons/ICON0.png
PSP_EBOOT_PIC1   = icons/Monkey64.png
BUILD_PRX        = 1
PSP_LARGE_MEMORY = 0

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak
