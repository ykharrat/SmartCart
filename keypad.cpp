#include "kaypad.h"
#include "mbed.h"
#include <cstdio>

// array of keys used on the keyPad
// you can modify this if you like
char keys[16] = {'1','2','3','A',
                 '4','5','6','B',
                 '7','8','9','C',
                 '*','0','#','D'};

// Create a BufferedSerial object with a default baud rate
static BufferedSerial pc(USBTX, USBRX);

// code for debounce timer used later in the lab
Timer debounce;
using namespace std::chrono;

// YOUR JOB:
// Declare additional variables for keyPad as needed

// NOTE:
// All functions currently return dummy values so the code will 
// compile while you are working. Be sure to update the return values!


// function used to setup the board at the beginning of main()
void setupBoard(void){

    // needed to use thread_sleep_for in debugger
    // your board will get stuck without it :(
    #if defined(MBED_DEBUG) && DEVICE_SLEEP
        HAL_DBGMCU_EnableDBGSleepMode();
    #endif

    // YOUR JOB
    // Initialize keyPad (as needed)
}

// perform one of four operations on num1, num2 using given operation
// A = addition
// B = subtraction
// C = multiplication
// D = division
// return the result as a float
// If the operation is invalid, return nanf("");
float computation(int num1, int num2, char operation){
    switch (operation) {
        case 'A':
            return num1 + num2 ;
            break;
        case 'B':
            return num1 - num2;
            break;
        case 'C':
            return num1 * num2;
            break;
        case 'D':
            return (float)num1/ num2;
            break;
        default:
            return nanf("");
    }
}

// Get a number from the serial terminal or keyPad. Return 
// the absolute value of the number as an int.
// If the number is invalid, returns error code -1.
// Also returns the operation and whether or not the number is negative.
int getNumber(int number, bool keyPad, char* operation, bool* negative){
    char buff = getCharacter(keyPad);
    int result;
    //checking for negative
    if (buff == '*') { 
        *negative = true;
        buff = getCharacter(keyPad);
        result = (buff - 0x30);
    } else {
        *negative = false;
        //if we didn't write in a *, we need a number whether it's first number or second one
        if ((buff >= 0x30) && (buff <= 0x39)) {
            result = (buff - 0x30);
        } else {
            return -1;
        }
    }
    //at this point we need to either keep writing in numbers or write in the operation or # if it's second number.
    buff = getCharacter(keyPad);
    while ((buff >= 0x30) && (buff <= 0x39)){
        result = result * 10 + (buff - 0x30);
        buff = getCharacter(keyPad);
    }
    if (number == 1) {
        switch (buff) {
            case 'A':
                *operation = 'A';
                break;
            case 'B':
                *operation = 'B';
                break;
            case 'C':
                *operation = 'C';
                break;
            case 'D':
                *operation = 'D';
                break;
            default:
                return -1;
        }
    } else if (number == 2) {
        if (buff == '#') {
            return result;
        } else {
            return -1;
        }
    }
    return result;
}

// Scan the keyPad for a valid keypress and return it as a character variable.
// Input the key mapping as an array.
// The keyPad will need to be debounced using the debounce timer.
char getKeyPress(char keys[]){
// BusIn rows (PB_7, PC_13, PC_14, PC_15);
// BusOut cols (PH_0, PH_1, PC_2, PC_3);
    // THE FIRST WAY OF DOING THIS

//     DigitalIn row1(PB_7, PullUp);
//     DigitalIn row2(PC_13,PullUp);
//     DigitalIn row3(PC_14,PullUp);
//     DigitalIn row4(PC_15,PullUp);

//     DigitalOut col1(PH_0);
//     DigitalOut col2(PH_1);
//     DigitalOut col3(PC_2);
//     DigitalOut col4(PC_3);
//     col1= 1;
//     col2= 1;
//     col3 = 1;
//     col4 = 1;

//     DigitalOut* col[] = {&col1, &col2, &col3, &col4};
//     DigitalOut* currentCol; 
//     DigitalIn* row[] = {&row1, &row2, &row3, &row4};
//     DigitalIn* currentRow;
//     while(1){
//         for (int i = 0; i<4 ; i++) {
//             currentCol = col[i];
//             *currentCol = 0;
//             for (int j = 0; j<4; j++){
//                 currentRow = row[j]; 
//                 if (!*currentRow){
//                     debounce.start();
//                     while (duration_cast<milliseconds>(debounce.elapsed_time()).count() < 400);
//                     debounce.stop();
//                     debounce.reset();
//                     return keys[j* 4 + i];
//                 }
//             }
//             *currentCol = 1;
//         }
//     }
// }


//     //THE SECOND WAY OF DOING THIS

    BusIn busIn(PC_10, PC_12, PA_13, PA_14);
    busIn.mode(PullUp);
    BusOut busOut(PA_15, PB_7, PC_13, PC_3);
    busOut.write(0b1111);
    int numberOld = 0;
    while(1){
        for (int i = 0; i<4; i++){
            busOut.write(15 - (1<<i));
            if(busIn.read() != 15){
                int number = 15 - busIn.read();
                int j = -1;
                bool test = false; 
                while(j<4 && (!test)) {
                    j++;
                    test = ~busIn.read() & 1<<j;
                }
                debounce.start();
                while (duration_cast<milliseconds>(debounce.elapsed_time()).count() < 400);
                debounce.stop();
                debounce.reset();
                return keys[j*4 + i]; 
            }
        }
    }
}

// Get a single character from the keyPad (keyPad = true) 
// or serial monitor (keyPad = false).
// Code should also "echo" the character to the serial terminal
// using pc.write(&buff,1);
char getCharacter(bool keyPad){
    //right now only reads and writes from serial monitor.
    char buff;
    if (keyPad) { 
        char buff = getKeyPress(keys);
        pc.write(&buff,1);
        return buff;
    } else {
        pc.read(&buff, 1);
        pc.write(&buff,1);
        return buff;
    }
}
