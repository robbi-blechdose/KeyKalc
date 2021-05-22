#include <SDL/SDL.h>
#include "SDL_gfx/SDL_framerate.h"

#include "display.h"
#include "calculator.h"

FPSmanager fpsManager;

//TODO: Move to its own file: "input"
#define INPUT_LENGTH 256
char input[INPUT_LENGTH];
uint16_t inputIndex = 0;

uint8_t running = 1;
SDL_Event event;

SDL_Surface* screen;

int main(int argc, char **argv)
{
    //Setup SDL
    SDL_Init(SDL_INIT_VIDEO);
    screen = SDL_SetVideoMode(240, 240, 16, 0);
    SDL_ShowCursor(SDL_DISABLE);
    //Setup framerate
    SDL_initFramerate(&fpsManager);
    SDL_setFramerate(&fpsManager, 50);
    //Setup display
    initDisplay();

    drawOSK(screen);

    //Main loop
    while(running)
    {
        //Read input
        while(SDL_PollEvent(&event))
        {
            if(event.type == SDL_QUIT)
            {
                printf("quit\n");
                running = 0;
            }
            else if(event.type == SDL_KEYUP)
            {
                switch(event.key.keysym.sym)
                {
                    case SDLK_u:
                    {
                        moveOSKCursor(screen, DIR_U);
                        break;
                    }
                    case SDLK_d:
                    {
                        moveOSKCursor(screen, DIR_D);
                        break;
                    }
                    case SDLK_l:
                    {
                        moveOSKCursor(screen, DIR_L);
                        break;
                    }
                    case SDLK_r:
                    {
                        moveOSKCursor(screen, DIR_R);
                        break;
                    }
                    case SDLK_a:
                    {
                        input[inputIndex++] = getCurrentChar();
                        drawInput(screen, input, inputIndex);
                        break;
                    }
                    case SDLK_b:
                    {
                        inputIndex--;
                        drawInput(screen, input, inputIndex);
                        break;
                    }
                    case SDLK_s:
                    {
                        //Start calculation
                        parse(input, inputIndex, 0);
                        calculateResult(0);
                        drawResult(screen, getResult());
                        break;
                    }
                    case SDLK_k:
                    {
                        //Show menu
                        break;
                    }
                    case SDLK_q:
                    {
                        running = 0;
                        break;
                    }
                }
            }
        }

        SDL_Flip(screen);
        SDL_framerateDelay(&fpsManager);
    }

    SDL_Quit();

	return 0;
}