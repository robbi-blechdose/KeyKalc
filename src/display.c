#include <SDL/SDL.h>
#include "SDL_gfx/SDL_gfxPrimitives.h"
#include "SDL_gfx/SDL_gfxPrimitives_font.h"

#include "display.h"
#include "ftoa.h"

uint8_t cursorX, cursorY;
const char* keyboard[4] = {"123+-QWERTZUIOP",
                           "456*\366ASDFGHJKL ",
                           "789^\373YXCVBNM   ",
                           "0. ()          "};

void initDisplay()
{
    gfxPrimitivesSetFont(gfxPrimitivesFontdata, 8, 8);
    cursorX = 0;
    cursorY = 0;
}

void drawTitleBar(SDL_Surface* screen, char* mode)
{
    hlineRGBA(screen, 0, 240, 13, 0, 255, 0, 255);
    boxRGBA(screen, 0, 0, 240, 12, 0, 0, 0, 255);
    stringRGBA(screen, 36, 4, "KeyKalc | Mode:", 0, 255, 0, 255);
    stringRGBA(screen, 36 + 16 * 8, 4, mode, 0, 255, 0, 255);
}

void drawInput(SDL_Surface* screen, char* input, uint16_t inputCursor, uint16_t inputEnd)
{
    uint16_t i = 0;
    uint8_t x = 0;
    uint8_t y = 0;
    
    boxRGBA(screen, 0, 14, 240, 199, 0, 0, 0, 255);

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
            boxRGBA(screen, 4 + x * 8, 17 + y * 8, 4 + x * 8 + 8, 17 + y * 8 + 8, 0, 255, 0, 255);
            characterRGBA(screen, 4 + x * 8, 18 + y * 8, c, 0, 0, 0, 255);
        }
        else
        {
            characterRGBA(screen, 4 + x * 8, 18 + y * 8, c, 0, 255, 0, 255);
        }
        x++;
        if(x == 29)
        {
            x = 0;
            y++;
        }
    }
}

void drawResult(SDL_Surface* screen, double result)
{
    char buffer[128];
    ftoa(result, buffer, -1);
    stringRGBA(screen, 4, 188, buffer, 0, 255, 0, 255);
}

void drawPlotGrid(SDL_Surface* screen)
{
    uint8_t i;
    uint8_t graphCenterY = 14 + ((199 - 14) / 2);
    //Clear screen
    boxRGBA(screen, 0, 14, 240, 199, 0, 0, 0, 255);
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
}

//TODO: Add scaling, clamp values vertically
void drawFunction(SDL_Surface* screen, int8_t points[], uint8_t index)
{
    uint8_t i;
    uint8_t r, g, b;
    switch(index)
    {
        case 0:
        {
            r = 0;
            g = 255;
            b = 0;
            break;
        }
        case 1:
        {
            r = 255;
            g = 0;
            b = 0;
            break;
        }
        case 2:
        {
            r = 0;
            g = 0;
            b = 255;
            break;
        }
    }

    uint8_t offset = 14 + ((199 - 14) / 2);

    for(i = 1; i < 240; i++)
    {
        lineRGBA(screen, i - 1, offset - points[i - 1], i, offset - points[i], r, g, b, 255);
    }
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