#pragma once
#include "mbed.h"
#include <cstdint>
#include <vector>
#define speedKp 5
#define speedKi 5
#define positionKp 0.003
#define positionKi 0.003

class MotorData{
public:
    MotorData(unsigned int);
    const unsigned int ID;//from 1 to 8
    int16_t rotorAngle;
    int16_t rotationSpeed;
    int16_t torqueCurrent;
    int8_t motorTemperature;
};
class Controller{
public:
    Controller(MotorData&);
    void (*compute)(void);
private:
    int16_t lastError;
    int16_t effort;
    int16_t setPoint;
    void speedController();
    void positionController();
    MotorData* motorData;
};

class Motor{
public:
    Motor(unsigned int);

    MotorData data;

    int position;
    Controller controller;
};

class Picker {
public:
    Picker();
    void read();
    void write();
private:
    //Motor elevator[4];
    //Motor flipper[4];
    vector<Motor> elevator;//4 in total. contorl ID from 1 to 4
    vector<Motor> flipper;//4 in total. control ID from 5 to 8
    CAN cAN;
};
class Chassis {
public:
    Chassis();
    void read();
    void write();
private:
    //Motor wheel[4];
    vector<Motor> wheel;
    CAN cAN;
};