#include "c620.h"
#include <cstdint>
#include <vector>

Motor::Motor(unsigned int id): ID(id){
    rotorAngle = 0;
    rotationSpeed = 0;
    torqueCurrent = 0;
    motorTemperature = 0;
    effort = 0;
}


void Motor::speedControl(){
    static int lastSpeedError = 0;
    int speedError = speedSetPoint - rotationSpeed;
    effort = speedError * speedKp;
    lastSpeedError = speedError;
}

void Motor::positionControl(){
    int positionError = positionSetPoint - position;
    speedSetPoint = positionError * positionKp;
    if(abs(speedSetPoint) > 3000){
        speedSetPoint = sgn(speedSetPoint) * 3000;
    }
    speedControl();
}

void Motor::updatePosition(){
    static int lastAngle = rotorAngle;
    int angleChange = rotorAngle - lastAngle;
    lastAngle = rotorAngle;
    if(abs(angleChange) > 4000){
        position -= sgn(angleChange) * 8190;
    }
    position += angleChange;
}

Picker::Picker():cAN(PA_11, PA_12){
    cAN.frequency(1000000);
    for(int i = 1; i < 9; i++){
        motor.push_back(Motor(i));
    }
    sendingMsgFlipper.id = 0x200;           //identifer for motor 1 to 4
    sendingMsgFlipper.format = CANStandard; //Standard Frame
    sendingMsgFlipper.type = CANData;       //Data 
    sendingMsgFlipper.len = 8;              // Size = 8 byte
    sendingMsgElevator.id = 0x1FF;           //identifer for motor 5 to 8
    sendingMsgElevator.format = CANStandard; //Standard Frame
    sendingMsgElevator.type = CANData;       //Data 
    sendingMsgElevator.len = 8;              // Size = 8 byte
    state = 0;
};

void Picker::read(){
    if(cAN.read(msgRead)){
        int motorID = msgRead.id - 0x201;
        motor[motorID].rotorAngle = (int16_t)msgRead.data[0] << 8 | msgRead.data[1];
        motor[motorID].rotationSpeed = (int16_t)msgRead.data[2] << 8 | msgRead.data[3];
        motor[motorID].torqueCurrent = (int16_t)msgRead.data[4] << 8 | msgRead.data[5];
        motor[motorID].motorTemperature = msgRead.data[6];
    }
}

void Picker::write(){
    sendingMsgFlipper.data[0] = motor[0].effort >> 8;
    sendingMsgFlipper.data[1] = motor[0].effort;
    sendingMsgFlipper.data[2] = motor[1].effort >> 8;
    sendingMsgFlipper.data[3] = motor[1].effort;
    sendingMsgFlipper.data[4] = motor[2].effort >> 8;
    sendingMsgFlipper.data[5] = motor[2].effort;
    sendingMsgFlipper.data[6] = motor[3].effort >> 8;
    sendingMsgFlipper.data[7] = motor[3].effort;
    cAN.write(sendingMsgFlipper);
    /*
    sendingMsgElevator.data[0] = motor[4].effort >> 8;
    sendingMsgElevator.data[1] = motor[4].effort;
    sendingMsgElevator.data[2] = motor[5].effort >> 8;
    sendingMsgElevator.data[3] = motor[5].effort;
    sendingMsgElevator.data[4] = motor[6].effort >> 8;
    sendingMsgElevator.data[5] = motor[6].effort;
    sendingMsgElevator.data[6] = motor[7].effort >> 8;
    sendingMsgElevator.data[7] = motor[7].effort;
    cAN.write(sendingMsgElevator);
    */
}

bool Picker::findZero(){
    static vector<Timer>stoped(4);
    bool shouldGoToNextState = true;
    for (int i = 0; i < 4; i++) {
        if(duration_cast<milliseconds>(stoped[i].elapsed_time()).count() > 1000){
            motor[i].speedSetPoint = 0;
        }
        if(abs(motor[i].rotationSpeed) < 10 ){
            stoped[i].start();
        }else {
            stoped[i].reset();
            shouldGoToNextState = false;
        }
        motor[i].speedControl();
    }
    return shouldGoToNextState;
}

void Picker::loop(){
    read();
    //motor[0].speedControl();
    write();
}
