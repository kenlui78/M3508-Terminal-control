#pragma once
#include "mbed.h"
#include <cstdint>
#include <string>

enum UiState{
    startUpPage,
    mainManu,
    motorSelect,
    velocityInput
};

class MotorUI{
    public:
    MotorUI();
    int16_t GetVelocityValue();
    private:
    UiState uiState;
    BufferedSerial pc;
    Thread pcIn;
    //Ticker chater;
    void messageTxRx(void);
    //void chat(void);
    int16_t velocityValue;
    char velocityString[8];
    int velocityStringCounter;
    std::string errorMessage;
    char newlineMessage;
    std::string mainManuMessage;
    std::string notCompletedMessage;
};