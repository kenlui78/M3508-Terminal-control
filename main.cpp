#include "mbed.h"
#include <chrono>
#include <cstdio>
#include "lagoriPicker.h"

using namespace std::chrono;


unsigned char g_stage = 0;

DigitalOut onBoardLed(LED1);

int main()
{
    LagoriPicker lagoriPicker;
    Timer motorUpdate;
    Timer pcUpdate;
    motorUpdate.start();
    pcUpdate.start();

    while (1) {
        lagoriPicker.pcIn();
        if(duration_cast<milliseconds>(motorUpdate.elapsed_time()).count() > 1){
            lagoriPicker.PIControl();
            motorUpdate.reset();
        }
        if(duration_cast<milliseconds>(pcUpdate.elapsed_time()).count() > 500){
            lagoriPicker.pcOut();
            pcUpdate.reset();
        }
    }
}

