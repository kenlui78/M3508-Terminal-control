#include "mbed.h"
#include <cstdio>
#include "lagoriPicker.h"

using namespace std::chrono;


unsigned char g_stage = 0;

DigitalOut onBoardLed(LED1);

Timer g_userTimer;
void changeStage(){
    if (duration_cast<milliseconds>(g_userTimer.elapsed_time()).count() > 1000) {
        //printf("Change stage\n");
        g_userTimer.reset();
        onBoardLed = !onBoardLed;
        switch (g_stage) {
        case 0:
            //printf("stage 0\n");
            g_stage = 1;
        break;
        case 1:
            //printf("Change stage 1\n");
            g_stage = 0;
        break;
        default:
        break;
        }
    }
}

void currentDrive(LagoriPicker* lagoriPicker){

}



int main()
{
    CAN ptrPickerCAN(PA_11, PA_12, 1000000);
    LagoriPicker lagoriPicker(&ptrPickerCAN);
    g_userTimer.start();
    InterruptIn userButton(PC_13);
    userButton.rise(&changeStage);

    while (true) {
        ThisThread::sleep_for(1ms);
    }
}

