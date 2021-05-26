CC          	= gcc

CFLAGS			= -Wall -fno-common -std=gnu99
LDFLAGS     	= -lc -lgcc -lm -lSDL -lSDL_image -lSDL_mixer -lSDL_ttf -lm -lstdc++ -lz

PNAME			= main.elf

DYNAMIC			= -lSDL_mixer -lSDL_ttf -lSDL_image -lSDL -lm -lmikmod
SDL_INCLUDE		= -I/opt/funkey-sdk/arm-funkey-linux-musleabihf/sysroot/usr/include/SDL  -D_GNU_SOURCE=1 -D_REENTRANT
SDL_GFX_INCLUDE = /home/julius/Dokumente/SDL_gfx/src
INCLUDES		= -I/opt/funkey-sdk/include $(SDL_INCLUDE) $(SDL_GFX_INCLUDE)

ifdef TARGET
include $(TARGET).mk
endif

# Files to be compiled
SRCDIR 		=  ./src
VPATH		= $(SRCDIR)
SRC_C		= $(foreach dir, $(SRCDIR), $(wildcard $(dir)/*.c))
OBJS		= $(notdir $(patsubst %.c, %.o, $(SRC_C)))

# Rules to make executable
$(PNAME): $(OBJS)
	$(CC) $(CFLAGS) -o $(PNAME) $^ $(LDFLAGS) $(SDL_GFX) $(DYNAMIC)

$(OBJS): %.o : %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c -o $@ $<

clean:
	rm -r build/*

oclean:
	rm *.o