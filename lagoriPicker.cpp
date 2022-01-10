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
    for(int i = 0; i < 4; i++){
    picker.profile_velocity_CCW[i] = 12000;//Maximum is 12000 for c620 and 10000 for c610
    picker.profile_velocity_CW[i] = -12000;
    picker.profile_torque_CCW[i] = 16000; //Maximum is 16000 for c620 and 10000 for c610
    picker.profile_torque_CW[i] = -16000;
    }
    // set the current limit, overcurrent may destory the driver
    picker.motor_max_current = 10000; // 10000 max for c610+2006 16000 max for c620
    picker.c620_read();

    pc.set_baud(115200);
    for(int i = 0; i < 4; i++){
    picker.global_pos[i] = 0;
    lastPosition[i] = picker.read_position[i];
    }
    targetVelocity = 0;
    outputCurrent = 0;
    inputValue = 0;
    controlType = 0;
    targetMotor = 0;
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
        case 'm':
            targetMotor = atoi(inputString);
            inputValue = 0;
            inputStringCounter = 0;
            memset(inputString, 0, 8);
            while(!pc.writable());
            printf("\n");
            while(!pc.writable());
            printf("Target motor = %d\n", targetMotor);
            valueIsEnter = true;
            controlType = 1;
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
            if(picker.read_velocity[targetMotor] == 0){
                Timer startupTime;
                startupTime.start();/*
                while (chrono::duration_cast<std::chrono::microseconds>(startupTime.elapsed_time()).count() < 5) {
                    picker.CAN_Send(1000 * sgn(targetVelocity), 0, 0, 0, 0, 0, 0, 0);
                }*/
            }
        }
        valueIsEnter = false;
    }
}

void LagoriPicker::PIControl(){
    picker.c620_read();
    int presentValue = 0;
    const float velocityKp = 5;
    const float velocityKi = 5;
    const float positionKp = 0.003;
    const float positionKi = 0.003;
    switch (controlType) {
        case 1:
        targetVelocity = inputValue;
        break;
        case 2:{
            int targetPosition = inputValue;
            int positionerror = (targetPosition - picker.global_pos[targetMotor]);
            int positionRegulate = positionerror * 0.1;
                if(abs(positionRegulate) > 3000)
                    positionRegulate = sgn(positionRegulate) * 3000;
                targetVelocity = positionRegulate;
        break;
        }
        default:
        targetVelocity = 0;
        break;
    }
    /*
    outputCurrent += (targetValue - picker.read_velocity[0]) * Kp;
    if(abs(targetValue) > 0 && abs(outputCurrent) < 150)
        outputCurrent = 150 * sgn(targetValue);
    if(targetValue == 0)
        outputCurrent = 0;
        */
    if(abs(picker.read_current[targetMotor]) > 16000){
        targetVelocity = 0;
        controlType = 1;
    }
    static int lastVelocityError = 0;
    int presentVelocityerror = (targetVelocity - picker.read_velocity[targetMotor]);
    outputCurrent += presentVelocityerror * velocityKp - lastVelocityError * velocityKi;
    lastVelocityError = presentVelocityerror;
    int currentOut[3] = {0};
    currentOut[targetMotor] = outputCurrent;
    picker.CAN_Send(currentOut[0], currentOut[1], currentOut[2], 0, 0, 0, 0, 0);


    int positionChange = picker.read_position[targetMotor] - lastPosition[targetMotor];
    lastPosition[targetMotor] = picker.read_position[targetMotor];
    if(abs(positionChange) > 6000){
        positionChange += 8191;
    }
    picker.global_pos[targetMotor] += positionChange;
}

void LagoriPicker::pcOut(){
    while(!pc.writable());
    
    printf("velocity: %d, position: %d, current: %d, output current: %d, global position: %d\n", 
        picker.read_velocity[targetMotor], picker.read_position[targetMotor], 
        picker.read_current[targetMotor], outputCurrent, picker.global_pos[targetMotor]);
        
    //printf("%d %d\n", *time, picker.read_velocity[targetMotor]);
}

void LagoriPicker::setInitialPosition(){
    picker.global_pos[targetMotor] = picker.read_position[targetMotor];
    lastPosition[targetMotor] = picker.global_pos[targetMotor];
}