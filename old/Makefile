TARGET = psp_test
OBJS = main.o

# Diretórios de includes e bibliotecas
INCDIR = /home/caiomadeira/pspdev/psp/include
LIBDIR = /home/caiomadeira/pspdev/psp/lib

# Flags para compilação e linkagem
CFLAGS = -Wall 
LDFLAGS = 

# Bibliotecas necessárias
LIBS = -lpspdebug -lpspdisplay -lpspkernel -lpspgum -lpspgu -lpspge -lpspctrl

# Título do EBOOT
EXTRA_TARGETS = EBOOT.PBP
PSP_EBOOT_TITLE = TITULO

# Caminho para o SDK
PSPSDK = $(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak
