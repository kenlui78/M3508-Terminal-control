#pragma once
#include "mbed.h"
#include <cstdint>
#include <string>
#include "lagoriPicker.h"

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
    LagoriPicker lagoriPicker;
    UiState uiState;
    BufferedSerial pc;
    Thread thread;
    void mainThread(void);
    UiState inputVelocity(char);
    Ticker ticker;

    int16_t velocityValue;
    char velocityString[8];
    int velocityStringCounter;
    char newlineMessage;
    std::string errorMessage;
    std::string mainManuMessage;
    std::string notCompletedMessage;
};