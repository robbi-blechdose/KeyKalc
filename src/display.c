#include <SDL/SDL.h>
#include "SDL_gfx/SDL_gfxPrimitives.h"
#include "SDL_gfx/SDL_gfxPrimitives_font.h"

#include "display.h"
#include "ftoa.h"

uint8_t cursorX, cursorY;
const char* keyboard[4] = {"123+-QWERTZUIOP",
                           "456*\366ASDFGHJKL ",
                           "789^\373YXCVBNM   ",
                           "0. ()         "};

void initDisplay()
{
    gfxPrimitivesSetFont(gfxPrimitivesFontdata, 8, 8);
    cursorX = 0;
    cursorY = 0;
}

void drawInput(SDL_Surface* screen, char* input, uint16_t inputEnd)
{
    uint16_t i = 0;
    uint8_t x = 0;
    uint8_t y = 0;
    
    boxRGBA(screen, 0, 0, 240, 199, 0, 0, 0, 255);

    for(i = 0; i < inputEnd; i++)
    {
        characterRGBA(screen, 4 + x * 8, 4 + y * 8, input[i], 0, 255, 0, 255);
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

void drawOSK(SDL_Surface* screen)
{
    hlineRGBA(screen, 0, 240, 200, 0, 255, 0, 255);
    stringRGBA(screen, 4, 204, keyboard[0], 0, 255, 0, 255);
    stringRGBA(screen, 4, 212, keyboard[1], 0, 255, 0, 255);
    stringRGBA(screen, 4, 220, keyboard[2], 0, 255, 0, 255);
    stringRGBA(screen, 4, 228, keyboard[3], 0, 255, 0, 255);
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