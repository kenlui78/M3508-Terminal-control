#include "mbed.h"
#include <chrono>
#include <cstdio>
#include "c620.h"

using namespace std::chrono;

DigitalOut onBoardLed(LED1);


int main()
{
    //LagoriPicker lagoriPicker;
    Picker picker;
    Timer motorUpdate;
    Timer pcUpdate;
    motorUpdate.start();
    pcUpdate.start();
    while (1) {
        if(duration_cast<milliseconds>(motorUpdate.elapsed_time()).count() > 100){
            picker.loop();
            motorUpdate.reset();
        }
        if(duration_cast<milliseconds>(pcUpdate.elapsed_time()).count() > 100){
            printf("speed[0]: %d, angle[0]: %d, effort: %d, speedSetPoint: %d\n", 
                    picker.motor[0].rotationSpeed, picker.motor[0].rotorAngle, picker.motor[0].effort, picker.motor[0].speedSetPoint);
            pcUpdate.reset();
        }
    }
}
