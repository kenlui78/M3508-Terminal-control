#include "lagoriPicker.h"
#include "mbed.h"
#include "stdint.h"
#include <chrono>
#include <cmath>
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
    picker.c620_read();

    pc.set_baud(115200);

    picker.global_pos[0] = 0;
    initialPosition = picker.read_position[0];
    lastPosition = initialPosition;
    targetValue = 0;
    outputCurrent = 0;
    inputValue = 0;
    controlType = 0;
}

void LagoriPicker::pcIn(){
    static char buffer[32] = {0};
    static char inputString[32] = {0};
    static int inputStringCounter = 0;
    bool valueIsEnter = false;
    if(pc.readable()){
        pc.read(buffer, 1);
        pc.write(buffer, 1);
        char inputChar = buffer[0];
        switch(inputChar){
        case 'v':
            inputValue = atoi(inputString);
            inputStringCounter = 0;
            memset(inputString, 0, 8);
            while(!pc.writable());
            printf("\n");
            while(!pc.writable());
            printf("input velocity = %d\n", inputValue);
            valueIsEnter = true;
            controlType = 1;
        break;
        case 'p':
            inputValue = atoi(inputString);
            inputStringCounter = 0;
            memset(inputString, 0, 8);
            while(!pc.writable());
            printf("\n");
            while(!pc.writable());
            printf("input position = %d\n", inputValue);
            valueIsEnter = true;
            controlType = 2;
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
            //targetValue = inputValue;
            if(picker.read_velocity[0] == 0){
                Timer startupTime;
                startupTime.start();
                while (chrono::duration_cast<std::chrono::microseconds>(startupTime.elapsed_time()).count() < 5) {
                    picker.CAN_Send(1000 * sgn(targetValue), 0, 0, 0, 0, 0, 0, 0);
                }
            }
        }
        valueIsEnter = false;
    }
}

void LagoriPicker::PIControl(){
    picker.c620_read();
    int presentValue = 0;
    const float Kp = 5;
    const float Ki = 5;
    static int lastError = 0;
    switch (controlType) {
        case 1:
        targetValue = inputValue;
        break;
        case 2:{
            int positionError = inputValue - picker.global_pos[0];
                if(abs(positionError) > 1500)
                    positionError = 1500;
                targetValue = positionError;
        break;
        }
        default:
        targetValue = 0;
        break;
    }
    /*
    outputCurrent += (targetValue - picker.read_velocity[0]) * Kp;
    if(abs(targetValue) > 0 && abs(outputCurrent) < 150)
        outputCurrent = 150 * sgn(targetValue);
    if(targetValue == 0)
        outputCurrent = 0;
    if(abs(picker.read_current[0]) > 4000)
        targetValue = 4000;
    */
    int error = (targetValue - picker.read_velocity[0]);
    outputCurrent += error * Kp - lastError * Ki;
    lastError = error;
    picker.CAN_Send(outputCurrent, 0, 0, 0, 0, 0, 0, 0);

    int positionChange = picker.read_position[0] - lastPosition;
    lastPosition = picker.read_position[0];
    if(abs(positionChange) > 6000){
        positionChange += 8191;
    }
    picker.global_pos[0] += positionChange;
}

void LagoriPicker::pcOut(){
    while(!pc.writable());
    
    printf("velocity: %d, position: %d, current: %d, output current: %d, global position: %d\n", 
        picker.read_velocity[0], picker.read_position[0], 
        picker.read_current[0], outputCurrent, picker.global_pos[0]);
        
    //printf("%d %d\n", *time, picker.read_velocity[0]);
}
