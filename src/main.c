#include <SDL/SDL.h>
#include "SDL_gfx/SDL_framerate.h"

#include "display.h"
#include "calculator.h"
#include "input.h"

uint8_t running = 1;
SDL_Event event;

SDL_Surface* screen;
FPSmanager fpsManager;

#define MODE_CALC  0
#define MODE_GRAPH 1
#define MODE_GDISP 2
uint8_t mode;

//Calc mode input
#define MAX_INPUT_LENGTH 256
char input[MAX_INPUT_LENGTH];
uint16_t inputCursor = 0;
uint16_t inputEnd = 0;

//Plot mode inputs
char plotInput0[MAX_INPUT_LENGTH];
char plotInput1[MAX_INPUT_LENGTH];
char plotInput2[MAX_INPUT_LENGTH];
char plotInput3[MAX_INPUT_LENGTH];
char* plotInputs[4] = {plotInput0, plotInput1, plotInput2, plotInput3};
uint16_t plotInputCursors[4] = {0, 0, 0, 0};
uint16_t plotInputEnds[4] = {0, 0, 0, 0};
uint8_t plotIndex = 0;

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
        drawPlotterInput(screen, plotInputs, plotInputCursors, plotInputEnds, plotIndex);
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
                        if(mode == MODE_CALC)
                        {
                            enterChar(input, &inputCursor, &inputEnd, getCurrentChar());
                            drawInput(screen, input, inputCursor, inputEnd);
                        }
                        else if(mode == MODE_GRAPH)
                        {
                            enterChar(plotInputs[plotIndex], &(plotInputCursors[plotIndex]),
                                        &(plotInputEnds[plotIndex]), getCurrentChar());
                            drawPlotterInput(screen, plotInputs, plotInputCursors, plotInputEnds, plotIndex);
                        }
                        break;
                    }
                    case SDLK_b:
                    {
                        if(mode == MODE_CALC)
                        {
                            removeChar(input, &inputCursor, &inputEnd);
                            drawInput(screen, input, inputCursor, inputEnd);
                        }
                        else if(mode == MODE_GRAPH)
                        {
                            removeChar(plotInputs[plotIndex], &(plotInputCursors[plotIndex]),
                                        &(plotInputEnds[plotIndex]));
                            drawPlotterInput(screen, plotInputs, plotInputCursors, plotInputEnds, plotIndex);
                        }
                        else if(mode == MODE_GDISP)
                        {
                            mode = MODE_GRAPH;
                            applyModeSwitch();
                        }
                        break;
                    }
                    case SDLK_s:
                    {
                        if(mode == MODE_CALC)
                        {
                            //Start calculation
                            parse(input, inputEnd);
                            calculateResult(0);
                            clearStack();
                            drawResult(screen, getResult());
                        }
                        else //if(mode == MODE_GRAPH)
                        {
                            mode = MODE_GDISP;
                            uint8_t i, j;
                            drawPlotGrid(screen);
                            for(i = 0; i < 4; i++)
                            {
                                if(plotInputEnds[i] == 0)
                                {
                                    continue;
                                }
                                double points[240];
                                parse(plotInputs[i], plotInputEnds[i]);
                                for(j = 0; j < 240; j++)
                                {
                                    calculateResult(((double) j - 120.0) / 20.0);
                                    points[j] = getResult() * 20.0;
                                }
                                clearStack();
                                drawFunction(screen, points, i);
                            }
                        }
                        break;
                    }
                    case SDLK_x:
                    {
                        if(mode == MODE_CALC)
                        {
                            if(inputCursor < inputEnd)
                            {
                                inputCursor++;
                                drawInput(screen, input, inputCursor, inputEnd);
                            }
                        }
                        else if(mode == MODE_GRAPH)
                        {
                            if(plotInputCursors[plotIndex] < plotInputEnds[plotIndex])
                            {
                                plotInputCursors[plotIndex]++;
                                drawPlotterInput(screen, plotInputs, plotInputCursors, plotInputEnds, plotIndex);
                            }
                        }
                        break;
                    }
                    case SDLK_y:
                    {
                        if(mode == MODE_CALC)
                        {
                            if(inputCursor > 0)
                            {
                                inputCursor--;
                                drawInput(screen, input, inputCursor, inputEnd);
                            }
                        }
                        else if(mode == MODE_GRAPH)
                        {
                            if(plotInputCursors[plotIndex] > 0)
                            {
                                plotInputCursors[plotIndex]--;
                                drawPlotterInput(screen, plotInputs, plotInputCursors, plotInputEnds, plotIndex);
                            }
                        }
                        break;
                    }
                    case SDLK_k:
                    {
                        //Mode switch
                        if(mode == MODE_CALC)
                        {
                            mode = MODE_GRAPH;
                        }
                        else //if(mode == MODE_GRAPH)
                        {
                            mode = MODE_CALC;
                        }
                        applyModeSwitch();
                        break;
                    }
                    case SDLK_n:
                    {
                        if(mode == MODE_GRAPH && plotIndex < 3)
                        {
                            plotIndex++;
                            drawPlotterInput(screen, plotInputs, plotInputCursors, plotInputEnds, plotIndex);
                        }
                        break;
                    }
                    case SDLK_m:
                    {
                        if(mode == MODE_GRAPH && plotIndex > 0)
                        {
                            plotIndex--;
                            drawPlotterInput(screen, plotInputs, plotInputCursors, plotInputEnds, plotIndex);
                        }
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