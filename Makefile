TARGET = PSPGAME
OBJS = main.o 

CFLAGS = 
CXXFLAGS - $(CFGLAGS) -std=c++14 -fno-rtti


# PSP Stuff Config
BUILD_PRX = 1
PSP_FW_VERSION = 500
PSP_LARGE_MEMORY = 1

EXTRA_TARGETS = EBOOT.PBP
PSP_EBOOT_TITLE = TITULO
#PSP_EBOOT_ICON = ICON0.PNG

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak