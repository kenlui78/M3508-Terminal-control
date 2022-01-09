#include "c620.h"
#include <cstdint>
#include <vector>

MotorData::MotorData(unsigned int id): ID(id){
    rotorAngle = 0;
    rotationSpeed = 0;
    torqueCurrent = 0;
    motorTemperature = 0;
}

Controller::Controller(MotorData& pMotorData): motorData(&pMotorData){
    lastError = 0;
    effort = 0;
    setPoint = 0;
}

void Controller::speedController(){
    int error = setPoint - motorData->rotationSpeed;
    effort += error * speedKp - lastError * speedKi;
    lastError = error;
}

void Controller::positionController(){
    
}

Motor::Motor(unsigned int id):data(id){
    position = 0;
}

Picker::Picker():cAN(PB_8, PB_9){
    for(int i = 1; i < 5; i++){
        elevator.push_back(Motor(i));
    }
    for(int i = 5; i < 9; i++){
        flipper.push_back(i);
    }
};
