#ifndef _CALCULATOR_H
#define _CALCULATOR_H

#define TYPE_NUMBER 0

//Identifiers for parsing functions
#define F_SIN 1
#define F_COS 2
#define F_TAN 3

#define F_ASIN 4
#define F_ACOS 5
#define F_ATAN 6

#define F_SQRT 7
#define F_NRT 8

#define F_COT 9
#define F_SEC 10
#define F_CSC 11

#define F_SINH 12
#define F_COSH 13
#define F_TANH 14

#define F_LOG 15
#define F_LN  16

#define F_N_SIN 17
#define F_N_COS 18
#define F_N_TAN 19

#define F_N_ASIN 20
#define F_N_ACOS 21
#define F_N_ATAN 22

#define F_N_SQRT 23
#define F_N_NRT 24

#define F_N_COT 25
#define F_N_SEC 26
#define F_N_CSC 27

#define F_N_SINH 28
#define F_N_COSH 29
#define F_N_TANH 30

#define F_N_LOG 31
#define F_N_LN  32
//---------------------------------

void parse(char enteredText[], uint16_t lastTextPos);
void calculateResult(double x, double angleMode);
void clearStack();
double getResult();

#endif