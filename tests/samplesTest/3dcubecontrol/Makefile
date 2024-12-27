TARGET = cube
OBJS = 3dplan.o logo.o

INCDIR =
CFLAGS = -Wall
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS)

LIBDIR =
LDFLAGS =
LIBS= -lpspgum -lpspgu -lpspctrl -lpspdisplay -lpspkernel -lpspdebug

EXTRA_TARGETS = EBOOT.PBP
PSP_EBOOT_TITLE = NEW 3D CUBE CONTROL WITH DEBUG TEXT

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak

logo.o: logo.raw
	bin2o -i logo.raw logo.o logo