TARGET = gnu_debugger_print_test
OBJS = main.o

INCDIR =
CFLAGS = -Wall
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS)

LIBDIR =
LDFLAGS =
LIBS = -lpspgum -lpspgu -lpspctrl -lpspdisplay -lpspkernel -lpspdebug

EXTRA_TARGETS = EBOOT.PBP
PSP_EBOOT_TITLE = GNU Debugger Print Test

PSPSDK = $(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak
