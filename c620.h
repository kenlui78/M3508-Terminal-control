#pragma once
#include "mbed.h"
#include <cstdint>

enum eControllerMode{
    speed, position
};

class MotorDate{
    public:
    MotorDate();
    const int ID;
    int16_t rotorAngle;
    int16_t rotationSpeed;
    int16_t torqueCurrent;
    int8_t motorTemperature;
};
class Controller{
    public:
    Controller();
    private:
    int16_t lastSpeed;
    int16_t lastPosition;
    
    eControllerMode mode;
};

class Motor{
    public:
    Motor(int);

    MotorDate data;

    long long Position;
    Controller controller;

    private:
    const int speedKp;
    const int speedKi;
    const int speedKd;
    const int positionKp;
    const int positionKi;
    const int positionKd;
};
class Transceiver{
    public:
    Transceiver();
    void send();
    void read();
    MotorDate motorDataRead;
    MotorDate motorDataToBeSend;
    private:
    CAN can;
};
class Picker{
    public:
    Picker();
    Transceiver transceiver;
    Motor elevator[4];
    Motor flipper[4];
};
class Chassis{
    public:
    Chassis();
    Transceiver transceiver;
    Motor wheel[4];
};