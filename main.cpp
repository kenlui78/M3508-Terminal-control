#include "mbed.h"
#include <chrono>
#include <cstdio>
#include "c620.h"

using namespace std::chrono;

DigitalOut onBoardLed(LED1);

int storeChar(char inputChar){
    static char inputString[32] = {0};
    static int inputStringCounter = 0;
    static int inputValue = 0;
    bool valueIsEnter = false;
        switch(inputChar){
        case 'v':
            inputValue = atoi(inputString);
            inputStringCounter = 0;
            memset(inputString, 0, 8);
            valueIsEnter = true;
        break;
        default:
            inputString[inputStringCounter] = inputChar;
            inputStringCounter++;
            if (inputStringCounter >= 8) {
                inputStringCounter = 0;
                memset(inputString, 0, 8);
            }
        break;
    }
    return inputValue;
}
int main()
{
    Picker picker;
    Timer motorUpdate;
    Timer pcUpdate;
    motorUpdate.start();
    pcUpdate.start();
    picker.motor[0].speedSetPoint = 500;
    picker.motor[1].speedSetPoint = 500;

    while (1) {
        if(duration_cast<milliseconds>(motorUpdate.elapsed_time()).count() > 1){
            picker.read();
            picker.motor[0].speedControl();
            picker.motor[1].speedControl();
            picker.write();
            motorUpdate.reset();
        }
    }
}
