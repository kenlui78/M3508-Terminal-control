#pragma once
#include "mbed.h"
#include <cstdint>
#include <vector>
#define speedKp 1
#define speedKi 1
#define positionKp 0.003
#define positionKi 0.003

template <typename T> int sgn(T val) {
	return (T(0) < val) - (val < T(0));
}
using namespace std::chrono;


class Motor{
public:
Motor(unsigned int);
    const unsigned int ID;//from 1 to 8
    int16_t rotorAngle;
    int16_t rotationSpeed;
    int16_t torqueCurrent;
    int8_t motorTemperature;
    int position;
    int speedSetPoint;
    int effort;
    int positionSetPoint;
    int positionEffort;

    void speedControl();
    void positionControl();
    void updatePosition();
};

class Picker {
public:
    Picker();
    void read();
    void write();
    void loop();
    bool findZero();
    vector<Motor> motor;//8 in total. contorl ID from 1 to 8. 1 to 4 is the fliper. 5 to 8 is the elevator
    int state;
private:
    //Motor elevator[4];
    //Motor flipper[4];
    CAN cAN;
    CANMessage msgRead;
    CANMessage sendingMsgFlipper;
    CANMessage sendingMsgElevator;
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