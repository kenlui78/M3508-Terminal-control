#include "mbed.h"
#include "stdint.h"
#include "motorUI.h"

MotorUI::MotorUI() : pc(USBTX, USBRX){
    tx_state = silent;
    pc.baud(9600);
    pc.format(
        /* bits */ 8,
        /* parity */ SerialBase::None,
        /* stop bit */ 1
    );
    pc.attach(callback(this, &MotorUI::rxInterrupt));
}

void MotorUI::rxInterrupt(){
    char ch;
    if (pc.read(&ch, 1)) {
        pc.write(&ch, 1);
    }
}