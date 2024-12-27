TARGET = shadowprojection
OBJS = shadowprojection.o

INCDIR =
CFLAGS = -Wall
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS)

LIBDIR =
LDFLAGS =
LIBS = -lpspgum -lpspgu -lpspdisplay -lpspkernel -lpspge -lpspdebug -lpspdebug -lpspdisplay -lpspge -lpspctrl -lpspnet -lpspnet_apctl

EXTRA_TARGETS = EBOOT.PBP
PSP_EBOOT_TITLE = >> MY SHADOW PROJECTION MODIFIED

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak
