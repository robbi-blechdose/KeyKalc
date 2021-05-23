#include <SDL/SDL.h>
#include "SDL_gfx/SDL_framerate.h"

#include "display.h"
#include "calculator.h"

uint8_t running = 1;
SDL_Event event;

SDL_Surface* screen;
FPSmanager fpsManager;

#define MAX_INPUT_LENGTH 256
char input[MAX_INPUT_LENGTH];
uint16_t inputEnd = 0;
uint16_t inputCursor = 0;

#define MODE_CALC  0
#define MODE_GRAPH 1
uint8_t mode;

void applyModeSwitch()
{
    drawOSK(screen);

    if(mode == MODE_CALC)
    {
        drawTitleBar(screen, "Calc");
        drawInput(screen, input, inputCursor, inputEnd);
    }
    else
    {
        drawTitleBar(screen, "Plot");
    }
}

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
    //Setup mode
    mode = MODE_CALC;
    applyModeSwitch();

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
                        enterChar(input, &inputCursor, &inputEnd, getCurrentChar());
                        drawInput(screen, input, inputCursor, inputEnd);
                        break;
                    }
                    case SDLK_b:
                    {
                        removeChar(input, &inputCursor, &inputEnd);
                        drawInput(screen, input, inputCursor, inputEnd);
                        break;
                    }
                    case SDLK_s:
                    {
                        //Start calculation
                        parse(input, inputEnd);
                        calculateResult(0);
                        drawResult(screen, getResult());
                        break;
                    }
                    case SDLK_x:
                    {
                        if(inputCursor < inputEnd)
                        {
                            inputCursor++;
                            drawInput(screen, input, inputCursor, inputEnd);
                        }
                        break;
                    }
                    case SDLK_y:
                    {
                        if(inputCursor > 0)
                        {
                            inputCursor--;
                            drawInput(screen, input, inputCursor, inputEnd);
                        }
                        break;
                    }
                    case SDLK_k:
                    {
                        //Mode switch
                        if(mode == MODE_CALC)
                        {
                            mode = MODE_GRAPH;
                            applyModeSwitch();
                            //TODO: Remove this test
                            drawPlotGrid(screen);
                            int8_t points[240];
                            uint8_t i;
                            for(i = 0; i < 240; i++)
                            {
                                points[i] = 20.0f * sin((i - 120) / 20.0f);
                            }
                            drawFunction(screen, points, 0);
                            for(i = 0; i < 240; i++)
                            {
                                points[i] = 20.0f * cos((i - 120) / 20.0f);
                            }
                            drawFunction(screen, points, 1);
                        }
                        else //if(mode == MODE_GRAPH)
                        {
                            mode = MODE_CALC;
                            applyModeSwitch();
                        }
                        break;
                    }
                    case SDLK_n:
                    {
                        break;
                    }
                    case SDLK_m:
                    {
                        break;
                    }
                    case SDLK_q:
                    {
                        running = 0;
                        break;
                    }
                    default:
                    {
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