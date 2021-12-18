#include "mbed.h"
#include <cstdio>
#include "lagoriPicker.h"
#define MAXIMUM_BUFFER_SIZE                                                  32

using namespace std::chrono;


unsigned char g_stage = 0;
bool g_pcIn = false;
int g_pcSpeed = 0;
DigitalOut onBoardLed(LED1);

Timer g_userTimer;
static UnbufferedSerial serial_port(USBTX, USBRX);
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

void on_rx_interrupt()
{
    static char buf[10] = {0};
    static int bufferCounter = 0;
    char c;

    // Toggle the LED.
    onBoardLed = !onBoardLed;

    // Read the data to clear the receive interrupt.
    if (serial_port.read(&c, 1)) {
        g_pcSpeed = c - '0';
        serial_port.write(&c, 1);
    }
}


int main()
{
    CAN ptrPickerCAN(PA_11, PA_12, 1000000);
    LagoriPicker lagoriPicker(&ptrPickerCAN);
    g_userTimer.start();
    InterruptIn userButton(PC_13);
    userButton.rise(&changeStage);
    serial_port.baud(9600);
    serial_port.format(
        /* bits */ 8,
        /* parity */ SerialBase::None,
        /* stop bit */ 1
    );
    serial_port.attach(&on_rx_interrupt, SerialBase::RxIrq);

    while (true) {
        lagoriPicker.setVelocity(0, -1*g_pcSpeed);
        ThisThread::sleep_for(1ms);
    }
}

