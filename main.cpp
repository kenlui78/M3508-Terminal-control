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
    //LagoriPicker lagoriPicker;
    Picker picker;
    Timer motorUpdate;
    Timer pcUpdate;
    motorUpdate.start();
    pcUpdate.start();
    char buffer[20];
    BufferedSerial pc(USBTX, USBRX);
    while (1) {
        if(duration_cast<milliseconds>(motorUpdate.elapsed_time()).count() > 100){
            picker.loop();
            motorUpdate.reset();
        }
        if (pc.readable()) {
            pc.read(buffer, 1);
            picker.motor[0].effort = storeChar(buffer[0]);
        }
        if(duration_cast<milliseconds>(pcUpdate.elapsed_time()).count() > 100){
            //printf("speed[0]: %d, angle[0]: %d, effort: %d, speedSetPoint: %d\n", 
            //        picker.motor[0].rotationSpeed, picker.motor[0].rotorAngle, picker.motor[0].effort, picker.motor[0].speedSetPoint);
            printf("%d\n", picker.motor[0].rotationSpeed);
            pcUpdate.reset();
        }
    }
}
