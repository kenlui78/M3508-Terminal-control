#include "c620.h"
#include <cstdint>
#include <vector>

Motor::Motor(unsigned int id): ID(id){
    rotorAngle = 0;
    rotationSpeed = 0;
    torqueCurrent = 0;
    motorTemperature = 0;
}


void Motor::velocityControl(){
    static int lastSpeedError = 0;
    int speedError = speedSetPoint - rotationSpeed;
    speedEffort += speedError * speedKp - lastSpeedError * speedKi;
    lastSpeedError = speedError;
}

void Motor::positionControl(){
    int positionError = positionSetPoint - position;
    speedSetPoint = positionError * positionKp;
    if(abs(speedSetPoint) > 3000){
        speedSetPoint = sgn(speedSetPoint) * 3000;
    }
    velocityControl();
}


Picker::Picker():cAN(PB_8, PB_9){
    for(int i = 1; i < 9; i++){
        motor.push_back(Motor(i));
    }
    sendingMsgElevator.id = 0x200;           //identifer for motor 1 to 4
    sendingMsgElevator.format = CANStandard; //Standard Frame
    sendingMsgElevator.type = CANData;       //Data 
    sendingMsgElevator.len = 8;              // Size = 8 byte
    sendingMsgFlipper.id = 0x1FF;           //identifer for motor 5 to 8
    sendingMsgFlipper.format = CANStandard; //Standard Frame
    sendingMsgFlipper.type = CANData;       //Data 
    sendingMsgFlipper.len = 8;              // Size = 8 byte
};

void Picker::read(){
    cAN.read(msgRead);
    int motorID = 0x200 - msgRead.id;
    motor[motorID].rotorAngle = (int16_t)6;
    /*
    can1->read(receive_msg);
    int16_t actual_local_position = (int16_t)(receive_msg.data[0] << 8) | receive_msg.data[1];
    int16_t actual_velocity = (int16_t)(receive_msg.data[2] << 8) | receive_msg.data[3];
    int16_t actual_current = (int16_t)(receive_msg.data[4] << 8) | receive_msg.data[5];
    int motor_index = 0x00;*/
}
