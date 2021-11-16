#include "mbed.h"
#include <cstdio>
#include "lagoriPicker.h"
unsigned char g_stage = 0;
DigitalOut onBoardLed(LED1);

void changeStage(){
    onBoardLed = !onBoardLed;
    switch (g_stage) {
    case 0:
        g_stage = 1;
    break;
    case 1:
        g_stage = 0;
    break;
    default:
    break;
    }
}

int main()
{
    CAN ptrPickerCAN(PA_11, PA_12, 1000000);
    LagoriPicker lagoriPicker(&ptrPickerCAN);
    InterruptIn userButton(PC_13);
    userButton.rise(&changeStage);
    while (true) {
        lagoriPicker.demonstration(g_stage);
    }
}

