#pragma once
#include "mbed.h"
#include <cstdint>

enum Tx_state{
    silent,
    transmittingCurrent,
    transmittingVelocity,
    transmittingPosition,
};

class MotorUI{
    public:
    MotorUI();
    private:
    Tx_state tx_state;
    UnbufferedSerial pc;
    void rxInterrupt(void);
};