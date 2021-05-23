#ifndef _INPUT_H
#define _INPUT_H

#include <SDL/SDL.h>

void enterChar(char* input, uint16_t* inputCursor, uint16_t* inputEnd, char c);
void removeChar(char* input, uint16_t* inputCursor, uint16_t* inputEnd);

#endif