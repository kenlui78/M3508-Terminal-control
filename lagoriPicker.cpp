#include "lagoriPicker.h"
#include "mbed.h"
#include "stdint.h"
#define gearRatio 22.75
#define currentRation 819.2
LagoriPicker::LagoriPicker() : canBus(PA_11, PA_12, 1000000), pc(USBTX, USBRX){
    picker.m3508_init(&canBus);

    picker.set_i_pid_param(0, 1, 0.000, 0.000000); // Torque PID W1
    picker.set_v_pid_param(0, 1, 0.000, 0.000);  // Velocity PID W1
    picker.set_p_pid_param(0, 1, 0.0, 0.00);     // position PID W1

    // set the LP filter of the desire control
    picker.profile_velocity_CCW[0] = 12000;//Maximum is 12000 for c620 and 10000 for c610
    picker.profile_velocity_CW[0] = -12000;
    picker.profile_torque_CCW[0] = 16000; //Maximum is 16000 for c620 and 10000 for c610
    picker.profile_torque_CW[0] = -16000;
    // set the current limit, overcurrent may destory the driver
    picker.motor_max_current = 10000; // 10000 max for c610+2006 16000 max for c620
    picker.set_position(0, 0);// set starting position as 0

    pc.set_baud(115200);

    int targetValue = 0;
    int outputCurrent = 0;
}

void LagoriPicker::pcIn(){
    static char buffer[32] = {0};
    static char inputString[32] = {0};
    static int inputValue = 0;
    static int inputStringCounter = 0;
    bool valueIsEnter = false;
    if(pc.readable()){
        pc.read(buffer, 1);
        pc.write(buffer, 1);
        char inputChar = buffer[0];
        switch(inputChar){
        case '\r':
            inputValue = atoi(inputString);
            inputStringCounter = 0;
            memset(inputString, 0, 8);
            while(!pc.writable());
            printf("\n");
            while(!pc.writable());
            printf("input value = %d\n", inputValue);
            //return 1;
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

        if(valueIsEnter){
            Timer startupTime;
            startupTime.start();
            targetValue = inputValue;
            while (std::chrono::duration<float>(startupTime.elapsed_time()).count() < 5) {
                picker.CAN_Send(1000 * (targetValue > 0) - (targetValue < 0), 0, 0, 0, 0, 0, 0, 0);
            }
        }
    }
}

void LagoriPicker::PIControl(){
    picker.c620_read();
    outputCurrent += (float)(targetValue - picker.read_velocity[0]) * (float)0.05;
    if(targetValue > 0 && outputCurrent < 150)
        outputCurrent = 150;
    if(targetValue == 0)
        outputCurrent = 0;
    if(picker.read_current[0] > 4000)
        targetValue = 0;
    picker.CAN_Send(outputCurrent, 0, 0, 0, 0, 0, 0, 0);
}

void LagoriPicker::pcOut(){
    while(!pc.writable());
    printf("velocity: %d, position: %d, current: %d, output current: %d, total position: %d\n", 
        picker.read_velocity[0], picker.read_position[0], 
        picker.read_current[0], outputCurrent, picker.global_pos[0]);
}
