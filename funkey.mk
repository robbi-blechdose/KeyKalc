PNAME		= main-funkey.elf
CC			= /opt/funkey-sdk/usr/bin/arm-linux-gcc
CFLAGS		+= -O3 -fdata-sections -ffunction-sections -flto -DFUNKEY
LDFLAGS		+= -ldl -lasound -lpng -ljpeg -lmikmod -logg -lvorbisidec -lmpg123 -lfreetype -lbz2 -Wl,--as-needed -Wl,--gc-sections -flto -s