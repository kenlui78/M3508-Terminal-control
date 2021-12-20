#pragma once
#include "m3508Lib/m3508.h"
#include "mbed.h"
#include <cstdint>

class LagoriPicker{
    public:
    CAN canBus;
    LagoriPicker();
    //void demonstration(unsigned char);
    int16_t getCurrent(int);
    void setVelocity(int, int);
    private:
    m3508 picker;
};

