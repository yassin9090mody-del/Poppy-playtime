TARGET = FactoryHorror
OBJS = main.o

CFLAGS = -O2 -G0 -Wall
CXXFLAGS = $(CFLAGS)
ASFLAGS = $(CFLAGS)

EXTRA_TARGETS = EBOOT.PBP
PSP_EBOOT_TITLE = Factory Horror
PSP_EBOOT_ICON =

include $(PSPSDK)/lib/build.mak
