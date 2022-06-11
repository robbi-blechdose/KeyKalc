#include <SDL/SDL.h>
#include <math.h>
#include "SDL_framerate.h"

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
#define MODE_PREFS 3
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

//Settings
double angleMode = 1.0;
uint8_t zoomIndex = 3;

const double zoomLevels[6] = {
    0.125,
    0.25,
    0.5,
    1.0,
    2.0,
    4.0
};

void applyModeSwitch()
{
    drawOSK(screen);

    if(mode == MODE_CALC)
    {
        drawTitleBar(screen, "Calc");
        drawInput(screen, input, inputCursor, inputEnd);
    }
    else if(mode == MODE_GRAPH || mode == MODE_GDISP)
    {
        drawTitleBar(screen, "Plot");
        drawPlotterInput(screen, plotInputs, plotInputCursors, plotInputEnds, plotIndex);
    }
    else //if(mode == G_PREFS)
    {
        drawTitleBar(screen, "Prefs");
        drawSettings(screen, angleMode);
    }
}

void plotGraph()
{
    double zoom = zoomLevels[zoomIndex];
    drawPlotGrid(screen, zoom);
    for(uint8_t i = 0; i < 4; i++)
    {
        if(plotInputEnds[i] == 0)
        {
            continue;
        }
        double points[240];
        parse(plotInputs[i], plotInputEnds[i]);
        for(uint8_t j = 0; j < 240; j++)
        {
            calculateResult(((double) j - 120.0) / (20.0 * zoom), angleMode);
            points[j] = getResult() * 20.0 * zoom;
        }
        clearStack();
        drawFunction(screen, points, i);
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
                            char* str = getCurrentChar();
                            while(*str != '\0')
                            {
                                enterChar(input, &inputCursor, &inputEnd, *str++);
                            }
                            drawInput(screen, input, inputCursor, inputEnd);
                        }
                        else if(mode == MODE_GRAPH)
                        {
                            char* str = getCurrentChar();
                            while(*str != '\0')
                            {
                                enterChar(plotInputs[plotIndex], &(plotInputCursors[plotIndex]), &(plotInputEnds[plotIndex]), *str++);
                            }
                            drawPlotterInput(screen, plotInputs, plotInputCursors, plotInputEnds, plotIndex);
                        }
                        else if(mode == MODE_PREFS)
                        {
                            if(angleMode == 1.0)
                            {
                                angleMode = M_PI / 180.0;
                            }
                            else
                            {
                                angleMode = 1.0;
                            }
                            drawSettings(screen, angleMode);
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
                            calculateResult(0, angleMode);
                            clearStack();
                            drawResult(screen, getResult());
                        }
                        else if(mode == MODE_GRAPH)
                        {
                            mode = MODE_GDISP;
                            plotGraph();
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
                        else if(mode == MODE_GDISP)
                        {
                            if(zoomIndex < 5)
                            {
                                zoomIndex++;
                            }
                            plotGraph();
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
                        else if(mode == MODE_GDISP)
                        {
                            if(zoomIndex > 0)
                            {
                                zoomIndex--;
                            }
                            plotGraph();
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
                        else if(mode == MODE_GRAPH)
                        {
                            mode = MODE_PREFS;
                        }
                        else
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