#include "input.h"

void enterChar(char* input, uint16_t* inputCursor, uint16_t* inputEnd, char c)
{
    (*inputEnd)++;
    //Shift everything back
    for(uint16_t i = *inputEnd; i > *inputCursor; i--)
    {
        input[i] = input[i - 1];
    }
    input[(*inputCursor)++] = c;
}

void removeChar(char* input, uint16_t* inputCursor, uint16_t* inputEnd)
{
    if(*inputCursor > 0)
    {
        //Shift everything
        for(uint16_t i = *inputCursor - 1; i < *inputEnd; i++)
        {
            input[i] = input[i + 1];
        }
        (*inputEnd)--;
        (*inputCursor)--;
    }
}