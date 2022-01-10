#pragma once
#include "m3508Lib/m3508.h"
#include "mbed.h"
#include <cstdint>

template <typename T> int sgn(T val) {
	return (T(0) < val) - (val < T(0));
}

class LagoriPicker{
    public:
    LagoriPicker();
    void pcIn();
    void PIControl();
    void pcOut();
    void setInitialPosition();
    private:
    BufferedSerial pc;
    m3508 picker;
    CAN canBus;
    int targetMotor;
    int targetVelocity;
    int outputCurrent;
    int lastPosition[8];
    int inputValue;
    int controlType;  //0: do not move, 1: velocity, 2:position
};

