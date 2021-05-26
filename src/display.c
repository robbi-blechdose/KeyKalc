#include <SDL/SDL.h>
#include "SDL_gfxPrimitives.h"
#include "SDL_gfxPrimitives_font.h"

#include "display.h"
#include "ftoa.h"

uint8_t cursorX, cursorY;
const char* keyboard[4] = {"123+-QWERTZUIOP",
                           "456*\366ASDFGHJKL ",
                           "789^\373YXCVBNM   ",
                           "0.,()\343e        "};

const uint8_t plotColors[4][3] = {
    {0, 255, 0},
    {255, 0, 0},
    {0, 255, 255},
    {255, 255, 0}
};

void initDisplay()
{
    gfxPrimitivesSetFont(gfxPrimitivesFontdata, 8, 8);
    cursorX = 0;
    cursorY = 0;
}

void clearMain(SDL_Surface* screen)
{
    boxRGBA(screen, 0, 14, 240, 199, 0, 0, 0, 255);
}

void drawTitleBar(SDL_Surface* screen, char* mode)
{
    hlineRGBA(screen, 0, 240, 13, 0, 255, 0, 255);
    boxRGBA(screen, 0, 0, 240, 12, 0, 0, 0, 255);
    stringRGBA(screen, 36, 4, "KeyKalc | Mode:", 0, 255, 0, 255);
    stringRGBA(screen, 36 + 16 * 8, 4, mode, 0, 255, 0, 255);
}

void drawInputText(SDL_Surface* screen, char* input, uint16_t inputCursor, uint16_t inputEnd,
                    uint8_t xStart, uint8_t yStart, uint8_t lineWidth)
{
    uint16_t i = 0;
    uint8_t x = 0;
    uint8_t y = 0;

    for(i = 0; i <= inputEnd; i++)
    {
        char c;
        if(i < inputEnd)
        {
            c = input[i];
        }
        else
        {
            c = ' ';
        }

        if(i == inputCursor)
        {
            boxRGBA(screen, (xStart - 1) + x * 8, (yStart - 1) + y * 8,
                            (xStart - 1) + x * 8 + 8, (yStart - 1) + y * 8 + 8, 0, 255, 0, 255);
            characterRGBA(screen, xStart + x * 8, yStart + y * 8, c, 0, 0, 0, 255);
        }
        else
        {
            characterRGBA(screen, xStart + x * 8, yStart + y * 8, c, 0, 255, 0, 255);
        }
        x++;
        if(x == lineWidth)
        {
            x = 0;
            y++;
        }
    }
}

void drawInput(SDL_Surface* screen, char* input, uint16_t inputCursor, uint16_t inputEnd)
{
    clearMain(screen);
    drawInputText(screen, input, inputCursor, inputEnd, 4, 18, 29);
}

void drawResult(SDL_Surface* screen, double result)
{
    char buffer[128];
    ftoa(result, buffer, -1);
    stringRGBA(screen, 4, 188, buffer, 0, 255, 0, 255);
}

void drawPlotterInput(SDL_Surface* screen, char* inputs[], uint16_t* inputCursors, uint16_t* inputEnds, uint8_t plotIndex)
{
    uint8_t i;
    clearMain(screen);

    uint8_t boxHeight = (199 - 14) / 4;
    char func = 'f';

    for(i = 0; i < 4; i++)
    {
        roundedRectangleRGBA(screen, 14, 18 + (i * boxHeight),
                                    240 - 4, 18 + (boxHeight - 6) + (i * boxHeight), 3, 0, 255, 0, 255);
        characterRGBA(screen, 4, 18 + (boxHeight / 2) + (i * boxHeight) - 4, func++,
                        plotColors[i][0], plotColors[i][1], plotColors[i][2], 255);
        
        uint16_t cursor = 65535; //Out of bounds means no cursor will be drawn
        if(i == plotIndex)
        {
            cursor = inputCursors[i];
        }
        drawInputText(screen, inputs[i], cursor, inputEnds[i], 18, 22 + (i * boxHeight), 27);
    }
}

void drawPlotGrid(SDL_Surface* screen, double zoom)
{
    uint8_t i;
    uint8_t graphCenterY = 14 + ((199 - 14) / 2);
    clearMain(screen);
    //Horizontal grid lines
    for(i = 16; i < 199; i += 10)
    {
        hlineRGBA(screen, 0, 240, i, 64, 64, 64, 255);
    }
    //Vertical grid lines
    for(i = 0; i < 240; i += 10)
    {
        vlineRGBA(screen, i, 14, 199, 64, 64, 64, 255);
    }
    //X and Y axis
    hlineRGBA(screen, 0, 240, graphCenterY, 0, 255, 0, 255);
    vlineRGBA(screen, 240 / 2, 14, 199, 0, 255, 0, 255);
    //Markers on Y axis
    for(i = 26; i < 199; i += 20)
    {
        hlineRGBA(screen, 120 - 2, 120 + 2, i, 0, 255, 0, 255);
    }
    //Markers on X axis
    for(i = 0; i < 240; i += 20)
    {
        vlineRGBA(screen, i, graphCenterY - 2, graphCenterY + 2, 0, 255, 0, 255);
    }

    char zoomStr[20];
    ftoa(5.0 / zoom, zoomStr, -1);

    stringRGBA(screen, 208, graphCenterY + 4, zoomStr, 0, 255, 0, 255);
    stringRGBA(screen, 124, 24, zoomStr, 0, 255, 0, 255);
}

double clamp(double value, double low, double high)
{
    if(value < low)
    {
        return low;
    }
    else if(value > high)
    {
        return high;
    }
    else
    {
        return value;
    }
}

void drawFunction(SDL_Surface* screen, double points[], uint8_t index)
{
    uint8_t i;

    uint8_t y1, y2;
    uint8_t offset = 14 + ((199 - 14) / 2);

    for(i = 1; i < 240; i++)
    {
        y1 = (uint8_t) clamp(offset - points[i - 1], 13, 200);
        y2 = (uint8_t) clamp(offset - points[i], 13, 200);
        lineRGBA(screen, i - 1, y1, i, y2, plotColors[index][0], plotColors[index][1], plotColors[index][2], 255);
    }
    //Clear top and bottom bars
    hlineRGBA(screen, 0, 240, 13, 0, 255, 0, 255);
    hlineRGBA(screen, 0, 240, 200, 0, 255, 0, 255);
}

void drawOSK(SDL_Surface* screen)
{
    hlineRGBA(screen, 0, 240, 200, 0, 255, 0, 255);
    stringRGBA(screen, 4, 204, keyboard[0], 0, 255, 0, 255);
    stringRGBA(screen, 4, 212, keyboard[1], 0, 255, 0, 255);
    stringRGBA(screen, 4, 220, keyboard[2], 0, 255, 0, 255);
    stringRGBA(screen, 4, 228, keyboard[3], 0, 255, 0, 255);
    drawOSKCursor(screen, 1);
}

void drawOSKCursor(SDL_Surface* screen, uint8_t new)
{
    int x = 4 + cursorX * 8;
    int y = 204 + cursorY * 8;

    if(new)
    {
        boxRGBA(screen, x - 1, y - 1, x + 7, y + 7, 0, 255, 0, 255);
        characterRGBA(screen, x, y, keyboard[cursorY][cursorX], 0, 0, 0, 255);
    }
    else
    {
        boxRGBA(screen, x - 1, y - 1, x + 7, y + 7, 0, 0, 0, 255);
        characterRGBA(screen, x, y, keyboard[cursorY][cursorX], 0, 255, 0, 255);
    }
}

void moveOSKCursor(SDL_Surface* screen, uint8_t dir)
{
    drawOSKCursor(screen, 0);

    switch(dir)
    {
        case DIR_U:
        {
            if(cursorY > 0)
            {
                cursorY--;
            }
            else
            {
                cursorY = C_BOTTOM;
            }
            break;
        }
        case DIR_D:
        {
            if(cursorY < C_BOTTOM)
            {
                cursorY++;
            }
            else
            {
                cursorY = 0;
            }
            break;
        }
        case DIR_L:
        {
            if(cursorX > 0)
            {
                cursorX--;
            }
            else
            {
                cursorX = C_RIGHT;
            }
            break;
        }
        case DIR_R:
        {
            if(cursorX < C_RIGHT)
            {
                cursorX++;
            }
            else
            {
                cursorX = 0;
            }
            break;
        }
    }

    drawOSKCursor(screen, 1);
}

uint8_t getCurrentChar()
{
    return keyboard[cursorY][cursorX];
}

void drawSettings(SDL_Surface* screen, double angleMode)
{
    clearMain(screen);

    stringRGBA(screen, 4, 18, "Angle mode:", 0, 255, 0, 255);
    if(angleMode == 1.0)
    {
        stringRGBA(screen, 4 + 12 * 8, 18, "RAD", 0, 255, 0, 255);
    }
    else
    {
        stringRGBA(screen, 4 + 12 * 8, 18, "DEG", 0, 255, 0, 255);
    }
}