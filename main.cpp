#include "mbed.h"
#include <chrono>
#include <cstdio>
#include "c620.h"

using namespace std::chrono;

DigitalOut onBoardLed(LED1);


int main()
{
    //LagoriPicker lagoriPicker;
    Picker picker();
    Timer motorUpdate;
    Timer pcUpdate;
    motorUpdate.start();
    pcUpdate.start();
    
    while (1) {
        //lagoriPicker.pcIn();
        if(duration_cast<milliseconds>(motorUpdate.elapsed_time()).count() > 1){
            //lagoriPicker.PIControl();
            motorUpdate.reset();
        }
        if(duration_cast<milliseconds>(pcUpdate.elapsed_time()).count() > 200){
            //lagoriPicker.pcOut();
            pcUpdate.reset();
        }
    }
}
