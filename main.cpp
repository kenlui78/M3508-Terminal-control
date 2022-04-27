#include "mbed.h"
#include <chrono>
#include <cstdio>
#include "c620.h"

using namespace std::chrono;

DigitalOut onBoardLed(LED1);
void (*controlPointer)(Picker&);

void positionControl(Picker& picker){
    picker.motor[0].positionControl();
    picker.motor[1].positionControl();
    picker.motor[2].positionControl();
    picker.motor[0].updatePosition();
    picker.motor[1].updatePosition();
    picker.motor[2].updatePosition();
}
void speedControl(Picker& picker){
    picker.motor[0].speedControl();
    picker.motor[1].speedControl();
    picker.motor[2].speedControl();
    picker.motor[0].updatePosition();
    picker.motor[1].updatePosition();
    picker.motor[2].updatePosition();
}
void checkInputChar(char inputChar, Picker& picker){
    static char inputString[32] = {0};
    static int inputStringCounter = 0;
    static int inputValue = 0;
    bool valueIsEnter = false;
        switch(inputChar){
        case 's':
            inputValue = atoi(inputString);
            inputStringCounter = 0;
            memset(inputString, 0, 8);
            valueIsEnter = true;
            controlPointer = &speedControl;
            for(int i = 0; i < 3; i++){
                picker.motor[i].speedSetPoint = inputValue;
                picker.motor[i].lastSpeedError = 0;
            }
        break;
        case 'p':
            inputValue = atoi(inputString);
            inputStringCounter = 0;
            memset(inputString, 0, 8);
            valueIsEnter = true;
            controlPointer = &positionControl;
            for(int i = 0; i < 3; i++){
                picker.motor[i].positionSetPoint = inputValue;
                picker.motor[i].lastSpeedError = 0;
            }
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
}

int main()
{
    Picker picker;
    Timer motorUpdate;
    Timer pcUpdate;
    motorUpdate.start();
    pcUpdate.start();
    BufferedSerial pc(USBTX, USBRX, 115200);
    char buffer[10];
    controlPointer = &speedControl;
    while (1) {
        picker.read();
        if(duration_cast<milliseconds>(motorUpdate.elapsed_time()).count() > 1){
            controlPointer(picker);
            picker.write();
            motorUpdate.reset();
        }
        if(duration_cast<milliseconds>(pcUpdate.elapsed_time()).count() > 200){
            if(pc.readable()){
                pc.read(buffer, 1);
                checkInputChar(buffer[0], picker);
            }
            while (!pc.writable());
            pc.write(buffer, sizeof(buffer));
            while (!pc.writable());
            printf("\nmotor[0] angle: %d, speed: %d, position: %d\nmotor[1] angle: %d, speed: %d, position: %d\nmotor[2] angle: %d, speed: %d, position: %d\n", 
            picker.motor[0].rotorAngle, picker.motor[0].rotationSpeed, picker.motor[0].position, 
            picker.motor[1].rotorAngle, picker.motor[1].rotationSpeed, picker.motor[1].position, 
            picker.motor[2].rotorAngle, picker.motor[2].rotationSpeed, picker.motor[2].position);
            pcUpdate.reset();
        }
    }
}
