#pragma once
#include "m3508Lib/m3508.h"
#include "mbed.h"
#include "motorUI.h"
#include <cstdint>

class LagoriPicker{
    public:
    LagoriPicker(CAN*);
    void demonstration(unsigned char);
    void zero(void);
    int16_t getCurrent(int);
    void setVelocity(int, int);
    private:
    m3508 picker;
    MotorUI motorUI;
    Thread motorUpdater;
    void motorUpdate(void);
};

