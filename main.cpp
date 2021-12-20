#include "mbed.h"
#include <cstdio>
#include "motorUI.h"

using namespace std::chrono;


unsigned char g_stage = 0;

DigitalOut onBoardLed(LED1);

Timer g_userTimer;
void changeStage(){
    if (duration_cast<milliseconds>(g_userTimer.elapsed_time()).count() > 200) {
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
    MotorUI motorUi;
    g_userTimer.start();
    InterruptIn userButton(PC_13);
    userButton.rise(&changeStage);

    while (true) {
        ThisThread::sleep_for(1ms);
    }
}

