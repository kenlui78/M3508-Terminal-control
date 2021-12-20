#include "mbed.h"
#include "stdint.h"
#include "motorUI.h"
#include <cmath>
#include <cstdint>

MotorUI::MotorUI() : pc(USBTX, USBRX){
    uiState = startUpPage;
    pc.set_baud(19200);
    pc.set_format(
        /* bits */ 8,
        /* parity */ SerialBase::None,
        /* stop bit */ 1
    );
    thread.start(callback(this, &MotorUI::mainThread));
    errorMessage = "\rError   \n";
    newlineMessage = '\n';
    mainManuMessage = "Mode: ";//v: set velocity
    notCompletedMessage = "\n";
    //chater.attach(callback(this, &MotorUI::chat), 2.0);
    velocityStringCounter = 0;
    velocityValue = 0;
}
UiState MotorUI::inputVelocity(char inputChar){
    switch(inputChar){
    case '\r':
        velocityValue = atoi(velocityString);
        velocityStringCounter = 0;
        memset(velocityString, 0, 8);
        pc.write(&newlineMessage, 1);
        printf("input value = %d\n", velocityValue);
    break;
    default:
        velocityString[velocityStringCounter] = inputChar;
        velocityStringCounter++;
        if (velocityStringCounter >= 8) {
            velocityStringCounter = 0;
            memset(velocityString, 0, 8);
            pc.write(&errorMessage, errorMessage.length());
        }
    break;
    }
    return velocityInput;
}

void MotorUI::mainThread(){
    char buffer[32] = {0};
    //pc.write(&mainManuMessage, sizeof(mainManuMessage));
    printf("Select mode: \n");
    uiState = mainManu;
    while(1){
        if (uint32_t num = pc.read(buffer, sizeof(buffer))){
            pc.write(buffer, num);
            inputVelocity(buffer[0]);
        }
        lagoriPicker.setVelocity(0, velocityValue);
    }
}

int16_t MotorUI::GetVelocityValue(){
    return velocityValue;
}
