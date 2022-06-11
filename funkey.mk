PNAME		= main-funkey.elf
CC			= /opt/funkey-sdk/usr/bin/arm-linux-gcc
CFLAGS		+= -O3 -fdata-sections -ffunction-sections -flto
LDFLAGS		+= -ldl -lasound -lpng -ljpeg -lmikmod -lfreetype -lbz2 -Wl,--as-needed -Wl,--gc-sections -flto -s
SDL_GFX      = /home/julius/Documents/Projects/FunKey/SDL_gfx/build/SDL_gfx_fk.a