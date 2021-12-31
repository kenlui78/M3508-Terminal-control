#pragma once
#include "m3508Lib/m3508.h"
#include "mbed.h"
#include <cstdint>

class LagoriPicker{
    public:
    LagoriPicker();
    void pcIn();
    void PIControl();
    void pcOut();
    private:
    BufferedSerial pc;
    m3508 picker;
    CAN canBus;
    int targetValue;
    int outputCurrent;
};

