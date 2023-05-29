#ifndef __CALC__
#define __CALC__


// declaring functions used in calculator.cpp
// see calculator.cpp for more details
float computation(int num1, int num2, char operation);
int getNumber(int number, bool keyPad, char* operation, bool* negative);
char getCharacter(bool keyPad);
char getKeyPress(char keys[]);
void setupBoard(void);

#endif