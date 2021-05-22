#include <stdint.h>
#include <math.h>

#include "calculator.h"

#define MAX_STACK_SIZE 81
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

float angleMode = 1.0f;

typedef struct
{
    double number;
    //Used for the stack, so that it can also be an operator
    uint8_t type;
} stackElement;

uint16_t i;
uint16_t j;

stackElement stack[MAX_STACK_SIZE];
uint16_t stackSize;
uint8_t operatorStack[MAX_STACK_SIZE];
uint16_t operatorStackSize;

double evalStack[MAX_STACK_SIZE];
uint16_t evalStackSize;

double result;

void pushStack(double number, uint8_t type)
{
    if(stackSize < MAX_STACK_SIZE)
    {
        stack[stackSize].number = number;
        stack[stackSize].type = type;
        stackSize++;
    }
}

stackElement * topStack()
{
    if(stackSize > 0)
    {
        return &stack[stackSize - 1];
    }
    else
    {
        return 0;
    }
}

void popStack()
{
    if(stackSize > 0)
    {
        stackSize--;
    }
}

void clearStack()
{
    stackSize = 0;
}

void pushOperatorStack(uint8_t o)
{
    if(operatorStackSize < MAX_STACK_SIZE)
    {
        operatorStack[operatorStackSize] = o;
        operatorStackSize++;
    }
}

uint8_t topOperatorStack()
{
    if(operatorStackSize > 0)
    {
        return operatorStack[operatorStackSize - 1];
    }
    else
    {
        return 0;
    }
}

void popOperatorStack()
{
    if(operatorStackSize > 0)
    {
        operatorStackSize--;
    }
}

void pushEvalStack(double number)
{
    if(evalStackSize < MAX_STACK_SIZE)
    {
        evalStack[evalStackSize] = number;
        evalStackSize++;
    }
}

double topEvalStack()
{
    if(evalStackSize > 0)
    {
        return evalStack[evalStackSize - 1];
    }
    else
    {
        return 0;
    }
}

void popEvalStack()
{
    if(evalStackSize > 0)
    {
        evalStackSize--;
    }
}

void clearEvalStack()
{
    evalStackSize = 0;
}

float exponentialf(float x, float n)
{
    if(x < 0)
    {
        if(fmod(n, 2) != 0)
        {
            if(n > 0)
            {
                return -powf(-x, n);
            }
            else
            {
                return 1 / -powf(-x, n);
            }
        }
        else
        {
            if(n > 0)
            {
                return powf(-x, n);
            }
            else
            {
                return 1 / powf(-x, n);
            }
        }
    }
    else
    {
        if(n > 0)
        {
            return powf(x, n);
        }
        else
        {
            return 1 / powf(x, -n);
        }
    }
}

uint8_t getOpPrecedence(uint8_t op)
{
    if(op == '(' || op == ')')
    {
        return 0;
    }
    else if(op == '+' || op == '-')
    {
        return 1;
    }
    else if(op == '*' || op == 0xF6)
    {
        return 2;
    }
    else if(op == '^')
    {
        return 3;
    }
    else
    {
        return 0;
    }
}

uint8_t isOperator(uint8_t token)
{
    if(token == '+' || token == '-' || token == '*' || token == 0xF6 || token == '^')
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

uint8_t isOperatorLeftAssociative(uint8_t token)
{
    if(token == '^')
    {
        return 0;
    }
    else
    {
        return 1;
    }
}

uint8_t isFunction(uint8_t token)
{
    if(token > 0 && token <= 32)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

void extractNumber(uint8_t enteredText[], uint16_t inputIndex)
{
    double temp = 0;
    double fraction = 0.1f;
    uint8_t numberState = 0;
    
    for(j = i; j < inputIndex; j++)
    {
        //Digit of a number
        if(isdigit(enteredText[j]))
        {
            if(numberState == 0)
            {
                temp *= 10;
                temp += enteredText[j] - '0';
            }
            else
            {
                temp += (enteredText[j] - '0') * fraction;
                fraction /= 10;
            }
        }
        //Decimal point
        else if(enteredText[j] == '.')
        {
            numberState = 1;
        }
        //Number has ended
        else
        {
            //Sign check
            if(i >= 2)
            {
                if(enteredText[i - 1] == '-' && (enteredText[i - 2] == '(' || isOperator(enteredText[i - 2]) || enteredText[i - 2] == '='))
                {
                    temp *= -1;
                }
            }
            //If the sign is at the beginning of the entered text
            else if(i == 1)
            {
                if(enteredText[0] == '-')
                {
                    temp *= -1;
                }
            }
    
            pushStack(temp, TYPE_NUMBER);
            i = j - 1;
            return;
        }
    }
}

uint8_t extractFunction(uint8_t enteredText[], uint16_t lastTextPos)
{
    uint8_t sign = 0;
    
    //Sign check
    if(i >= 2)
    {
        if(enteredText[i - 1] == '-' && (enteredText[i - 2] == '(' || isOperator(enteredText[i - 2]) || enteredText[i - 2] == '='))
        {
            sign = 16;
        }
    }
    //If the sign is at the beginning of the entered text
    else if(i == 1)
    {
        if(enteredText[0] == '-')
        {
            sign = 16;
        }
    }
    
    //Check if it's an A-something function
    if(enteredText[i] == 'A')
    {
        i++;
        j = extractFunction(enteredText, lastTextPos);
        if(j != 0)
        {
            return j + 3;
        }
        return 0;
    }
    //SQRT
    else if(enteredText[i] == 0xFB)
    {
        if(enteredText[i + 1] == '(')
        {
            i++;
            return F_SQRT + sign;
        }
        else if(enteredText[i + 1] == 'N')
        {
            if(enteredText[i + 2] == '(')
            {
                i += 2;
                return F_NRT + sign;
            }
        }
        return 0;
    }
    //SIN / SINH / SEC
    else if(enteredText[i] == 'S')
    {
        if(enteredText[i + 1] == 'I')
        {
            if(enteredText[i + 2] == 'N')
            {
                if(enteredText[i + 3] == '(')
                {
                    i += 3;
                    return F_SIN + sign;
                }
                else if(enteredText[i + 3] == 'H')
                {
                    if(enteredText[i + 4] == '(')
                    {
                        i += 4;
                        return F_SINH + sign;
                    }
                }
            }
        }
        else if(enteredText[i + 1] == 'E')
        {
            if(enteredText[i + 2] == 'C')
            {
                if(enteredText[i + 3] == '(')
                {
                    i += 3;
                    return F_SEC + sign;
                }
            }
        }
        return 0;
    }
    //COS / COSH / COT / CSC
    else if(enteredText[i] == 'C')
    {
        if(enteredText[i + 1] == 'O')
        {
            if(enteredText[i + 2] == 'S')
            {
                if(enteredText[i + 3] == '(')
                {
                    i += 3;
                    return F_COS + sign;
                }
                else if(enteredText[i + 3] == 'H')
                {
                    if(enteredText[i + 4] == '(')
                    {
                        i += 4;
                        return F_COSH + sign;
                    }
                }
            }
            else if(enteredText[i + 2] == 'T')
            {
                if(enteredText[i + 3] == '(')
                {
                    i += 3;
                    return F_COT + sign;
                }
            }
        }
        else if(enteredText[i + 1] == 'S')
        {
            if(enteredText[i + 2] == 'C')
            {
                if(enteredText[i + 3] == '(')
                {
                    i += 3;
                    return F_CSC + sign;
                }
            }
        }
        return 0;
    }
    //TAN / TANH
    else if(enteredText[i] == 'T')
    {
        if(enteredText[i + 1] == 'A')
        {
            if(enteredText[i + 2] == 'N')
            {
                if(enteredText[i + 3] == '(')
                {
                    i += 3;
                    return F_TAN + sign;
                }
                else if(enteredText[i + 3] == 'H')
                {
                    if(enteredText[i + 4] == '(')
                    {
                        i += 4;
                        return F_TANH + sign;
                    }
                }
            }
        }
        return 0;
    }
    //LOG / LN
    else if(enteredText[i] == 'L')
    {
        if(enteredText[i + 1] == 'O')
        {
            if(enteredText[i + 2] == 'G')
            {
                if(enteredText[i + 3] == '(')
                {
                    i += 3;
                    return F_LOG + sign;
                }
            }
        }
        else if(enteredText[i + 1] == 'N')
        {
            if(enteredText[i + 2] == '(')
            {
                i += 2;
                return F_LN + sign;
            }
        }
        return 0;
    }
    else
    {
        return 0;
    }
}

void parse(uint8_t enteredText[], uint16_t inputIndex, uint16_t startPos)
{
    uint8_t tempInt = 0;
    
    /** -- Dijkstra's shunting yard algorithm (https://en.wikipedia.org/wiki/Shunting-yard_algorithm) -- */
    
    enteredText[inputIndex++] = '#';
    
    for(i = startPos; i < inputIndex; i++)
    {
        //Digit of a number
        if(isdigit(enteredText[i]))
        {
            //Extract the whole number
            extractNumber(enteredText, inputIndex);
        }
        //Operator
        else if(isOperator(enteredText[i]) && i != 0 && !isOperator(enteredText[i - 1]) && enteredText[i - 1] != '=' && enteredText[i - 1] != '(')
        {
            while(((isOperatorLeftAssociative(enteredText[i]) && getOpPrecedence(topOperatorStack()) >= getOpPrecedence(enteredText[i])) ||
                    getOpPrecedence(topOperatorStack()) > getOpPrecedence(enteredText[i])) && topOperatorStack() != '(')
            {
                pushStack(0, topOperatorStack());
                popOperatorStack();
            }
            
            pushOperatorStack(enteredText[i]);
        }
        else if(enteredText[i] == '(')
        {
            pushOperatorStack(enteredText[i]);
        }
        else if(enteredText[i] == ')')
        {
            while(topOperatorStack() != 0 && !(topOperatorStack() == '(' || isFunction(topOperatorStack())))
            {
                pushStack(0, topOperatorStack());
                popOperatorStack();
            }
            
            //Pop away opening braces (they're pointless in postfix)
            if(!isFunction(topOperatorStack()))
            {
                popOperatorStack();
            }
            //Pop function onto stack
            else
            {
                pushStack(0, topOperatorStack());
                popOperatorStack();
            }
        }
        //ANS = Last result
        else if(enteredText[i] == 'A' && enteredText[i + 1] == 'N' && enteredText[i + 2] == 'S')
        {
            i += 2;
            pushStack(result, TYPE_NUMBER);
        }
        //For the function plotter: Include X in postfix!
        //Use Y for when X has a minus in front of it
        else if(enteredText[i] == 'X')
        {
            if(enteredText[i - 1] == '-')
            {
                pushStack(0, 'Y');
            }
            else
            {
                pushStack(0, 'X');
            }
        }
        //A function
        else if(isupper(enteredText[i]) || enteredText[i] == 0xFB)
        {
            tempInt = extractFunction(enteredText, inputIndex);
            if(tempInt != 0)
            {
                pushOperatorStack(tempInt);
            }
        }
        //Pi detected
        else if(enteredText[i] == 0xE3)
        {
            pushStack(M_PI, TYPE_NUMBER);
        }
        else if(enteredText[i] == 'e')
        {
            pushStack(M_E, TYPE_NUMBER);
        }
    }
    
    while(topOperatorStack() != 0)
    {
        pushStack(0, topOperatorStack());
        popOperatorStack();
    }
    
    enteredText[inputIndex] = 0;
    inputIndex--;
}

void calculateResult(double x)
{
    stackElement* stackTemp;
    double stackTemp2;
    double stackTemp3;
    
    double temp = 0;

    /** ------- RPN evaluation algorithm (https://en.wikipedia.org/wiki/Reverse_Polish_notation) ------- */
    for(i = 0; i != stackSize; i++)
    {
        stackTemp = &stack[i];
        
        if(stackTemp->type == TYPE_NUMBER)
        {
            pushEvalStack(stackTemp->number);
        }
        else if(stackTemp->type == 'X')
        {
            pushEvalStack(x);
        }
        else if(stackTemp->type == 'Y')
        {
            pushEvalStack(-x);
        }
        else
        {
            stackTemp2 = topEvalStack();
            popEvalStack();
            stackTemp3 = topEvalStack();
            popEvalStack();
                
            switch(stackTemp->type)
            {
                case '+':
                {
                    temp = stackTemp2 + stackTemp3;
                    break;
                }
                case '-':
                {
                    temp = stackTemp3 - stackTemp2;
                    break;
                }
                case '*':
                {
                    temp = stackTemp2 * stackTemp3;
                    break;
                }
                case 0xF6:
                {
                    temp = stackTemp3 / stackTemp2;
                    break;
                }
                case '^':
                {
                    temp = exponentialf(stackTemp3, stackTemp2);
                    break;
                }
                //Functions
                default:
                {
                    //Functions only use one number, so put the other one back onto the stack
                    if(stackTemp3 != 0 && stackTemp->type != F_LOG && stackTemp->type != F_N_LOG && stackTemp->type != F_NRT && stackTemp->type != F_N_NRT)
                    {
                        pushEvalStack(stackTemp3);
                    }
                    
                    switch(stackTemp->type)
                    {
                        case F_SIN:
                        {
                            temp = sin(stackTemp2 * angleMode);
                            break;
                        }
                        case F_N_SIN:
                        {
                            temp = -sin(stackTemp2 * angleMode);
                            break;
                        }
                        case F_COS:
                        {
                            temp = cos(stackTemp2 * angleMode);
                            break;
                        }
                        case F_N_COS:
                        {
                            temp = -cos(stackTemp2 * angleMode);
                            break;
                        }
                        case F_TAN:
                        {
                            temp = tan(stackTemp2 * angleMode);
                            break;
                        }
                        case F_N_TAN:
                        {
                            temp = -tan(stackTemp2 * angleMode);
                            break;
                        }
                        case F_ASIN:
                        {
                            temp = asin(stackTemp2 * angleMode);
                            break;
                        }
                        case F_N_ASIN:
                        {
                            temp = -asin(stackTemp2 * angleMode);
                            break;
                        }
                        case F_ACOS:
                        {
                            temp = acos(stackTemp2 * angleMode);
                            break;
                        }
                        case F_N_ACOS:
                        {
                            temp = -acos(stackTemp2 * angleMode);
                            break;
                        }
                        case F_ATAN:
                        {
                            temp = atan(stackTemp2 * angleMode);
                            break;
                        }
                        case F_N_ATAN:
                        {
                            temp = -atan(stackTemp2 * angleMode);
                            break;
                        }
                        case F_SQRT:
                        {
                            temp = sqrt(stackTemp2 * angleMode);
                            break;
                        }
                        case F_N_SQRT:
                        {
                            temp = -sqrt(stackTemp2 * angleMode);
                            break;
                        }
                        case F_NRT:
                        {
                            temp = exponentialf(stackTemp2, 1 / stackTemp3);
                            break;
                        }
                        case F_N_NRT:
                        {
                            temp = -exponentialf(stackTemp2, 1 / stackTemp3);
                            break;
                        }
                        case F_COT:
                        {
                            //temp = cot(stackTemp2 * angleMode);
                            break;
                        }
                        case F_N_COT:
                        {
                            //temp = -cot(stackTemp2 * angleMode);
                            break;
                        }
                        case F_SEC:
                        {
                            temp = 1 / cos(stackTemp2 * angleMode);
                            break;
                        }
                        case F_N_SEC:
                        {
                            temp = -(1 / cos(stackTemp2 * angleMode));
                            break;
                        }
                        case F_CSC:
                        {
                            temp = 1 / sin(stackTemp2 * angleMode);
                            break;
                        }
                        case F_N_CSC:
                        {
                            temp = -(1 / sin(stackTemp2 * angleMode));
                            break;
                        }
                        case F_SINH:
                        {
                            temp = sinh(stackTemp2 * angleMode);
                            break;
                        }
                        case F_N_SINH:
                        {
                            temp = -sinh(stackTemp2 * angleMode);
                            break;
                        }
                        case F_COSH:
                        {
                            temp = cosh(stackTemp2 * angleMode);
                            break;
                        }
                        case F_N_COSH:
                        {
                            temp = -cosh(stackTemp2 * angleMode);
                            break;
                        }
                        case F_TANH:
                        {
                            temp = tanh(stackTemp2 * angleMode);
                            break;
                        }
                        case F_N_TANH:
                        {
                            temp = -tanh(stackTemp2 * angleMode);
                            break;
                        }
                        case F_LOG:
                        {
                            temp = log(stackTemp2) / log(stackTemp3);
                            break;
                        }
                        case F_N_LOG:
                        {
                            temp = -(log(stackTemp2) / log(stackTemp3));
                            break;
                        }
                        case F_LN:
                        {
                            temp = log(stackTemp2);
                            break;
                        }
                        case F_N_LN:
                        {
                            temp = -log(stackTemp2);
                            break;
                        }
                    }
                    
                    break;
                }
            }
            
            pushEvalStack(temp);
        }
    }
    
    result = evalStack[0];
    clearEvalStack();
    clearStack();
}

double getResult()
{
    return result;
}