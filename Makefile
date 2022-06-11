CC              = gcc

CFLAGS          = -O3 -flto -Wall -fno-common -std=gnu99
LDFLAGS         = -lm -lSDL -lSDL_image -lm -lz

PNAME		    = main.elf

SDL_GFX_LIB     = /home/julius/Documents/Projects/FunKey/SDL_gfx/build/SDL_gfx.a

SDL_INCLUDE	    = -I/opt/funkey-sdk/arm-funkey-linux-musleabihf/sysroot/usr/include/SDL -D_GNU_SOURCE=1 -D_REENTRANT
SDL_GFX_INCLUDE = -I/home/julius/Documents/Projects/FunKey/SDL_gfx/src
INCLUDES	    = -I/opt/funkey-sdk/include $(SDL_INCLUDE) $(SDL_GFX_INCLUDE)

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
	$(CC) $(CFLAGS) -o $(PNAME) $^ $(LDFLAGS) $(SDL_GFX_LIB)

$(OBJS): %.o : %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c -o $@ $<

oclean:
	rm *.o