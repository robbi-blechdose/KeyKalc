#ifndef _CALCULATOR_H
#define _CALCULATOR_H

void parse(uint8_t enteredText[], uint16_t lastTextPos);
void calculateResult(double x);
void clearStack();
double getResult();

#endif