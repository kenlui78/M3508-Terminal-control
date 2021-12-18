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
    pcIn.start(callback(this, &MotorUI::messageTxRx));
    errorMessage = "\rError   \n";
    newlineMessage = '\n';
    mainManuMessage = "Mode: ";//z: finding zero, g: grip, r:return to zero
    notCompletedMessage = "咕\n";
    //chater.attach(callback(this, &MotorUI::chat), 2.0);
    velocityStringCounter = 0;
    velocityValue = 0;
}

void MotorUI::messageTxRx(){
    char buffer[32] = {0};
    pc.write(&mainManuMessage, sizeof(mainManuMessage));
    uiState = mainManu;
    while(1){
        if (uint32_t num = pc.read(buffer, sizeof(buffer))){
            pc.write(buffer, num);
            switch (uiState){
                case mainManu:
                    switch (buffer[0]) {
                    case 'g':
                        uiState = velocityInput;
                        break;
                    default:
                        pc.write(&notCompletedMessage, sizeof(notCompletedMessage));
                        break;
                    }
                case velocityInput:
                        if(buffer[0] == '\r'){
                            velocityValue = atoi(velocityString);
                            velocityStringCounter = 0;
                            memset(velocityString, 0, 8);
                            pc.write(&newlineMessage, 1);
                            printf("input value = %d\n", velocityValue);
                        }else{
                            velocityString[velocityStringCounter] = buffer[0];
                            velocityStringCounter++;
                            if (velocityStringCounter >= 8) {
                                velocityStringCounter = 0;
                                memset(velocityString, 0, 8);
                                pc.write(&errorMessage, errorMessage.length());
                            }
                        }
                break;
                default:
                    pc.write(&mainManuMessage, sizeof(mainManuMessage));
                    uiState = mainManu;
                    break;
            }
        }
    }
}

/*
                    if (uint32_t num = pc.read(buffer, sizeof(buffer))) {
                        // Echo the input back to the terminal.
                        pc.write(buffer, num);
                        if(buffer[0] == '\r'){
                            velocityValue = atoi(velocityString);
                            velocityStringCounter = 0;
                            memset(velocityString, 0, 8);
                            pc.write(&newlineMessage, 1);
                            printf("字符串值 = %s, 整型值 = %d\n", velocityString, velocityValue);
                        }else{
                            velocityString[velocityStringCounter] = buffer[0];
                            velocityStringCounter++;
                            if (velocityStringCounter >= 8) {
                                velocityStringCounter = 0;
                                memset(velocityString, 0, 8);
                                pc.write(&errorMessage, errorMessage.length());
                            }
                        }
                    }

*/

int16_t MotorUI::GetVelocityValue(){
    return velocityValue;
}

/*void MotorUI::chat(){
    static char buffer[9] = "working\n";
    pc.write(buffer, sizeof(buffer));
}*/
