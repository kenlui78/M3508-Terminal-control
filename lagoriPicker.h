#include "m3508Lib/m3508.h"
#include "mbed.h"
#include <cstdint>

class LagoriPicker{
    public:
    LagoriPicker(CAN*);
    void demonstration(unsigned char);
    private:
    m3508 picker;
};