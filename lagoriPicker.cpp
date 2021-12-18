#include "lagoriPicker.h"
#include "mbed.h"
#include "stdint.h"
#define gearRatio 22.75
#define currentRation 819.2
LagoriPicker::LagoriPicker(CAN* pickerCAN){
    picker.m3508_init(pickerCAN);

    //suggest position mode setting 
    picker.set_i_pid_param(0, 1.5, 0.001, 0.000005); // Torque PID W1
    picker.set_v_pid_param(0, 5.5, 0.001, 0.005);  // Velocity PID W1
    picker.set_p_pid_param(0, 12.5, 0.4, 0.35);     // position PID W1
    picker.set_i_pid_param(1, 1.5, 0.001, 0.000005); // Torque PID W1
    picker.set_v_pid_param(1, 5.5, 0.001, 0.005);  // Velocity PID W1
    picker.set_p_pid_param(1, 12.5, 0.4, 0.35);     // position PID W1
    picker.set_i_pid_param(2, 1.5, 0.001, 0.000005); // Torque PID W1
    picker.set_v_pid_param(2, 5.5, 0.001, 0.005);  // Velocity PID W1
    picker.set_p_pid_param(2, 12.5, 0.4, 0.35);     // position PID W1

    // set the LP filter of the desire control
    picker.profile_velocity_CCW[0] = 12000;//Maximum is 12000 for c620 and 10000 for c610
    picker.profile_velocity_CW[0] = -12000;
    picker.profile_torque_CCW[0] = 16000; //Maximum is 16000 for c620 and 10000 for c610
    picker.profile_torque_CW[0] = -16000;
    // set the current limit, overcurrent may destory the driver
    picker.motor_max_current = 16000; // 10000 max for c610+2006 16000 max for c620
    picker.set_position(0, 0);// set starting position as 0
}
void LagoriPicker::demonstration(unsigned char stage){
    switch (stage) {
    case 0:
        picker.set_position(0, 0 * gearRatio);
        picker.set_position(1, 0 * gearRatio);
        picker.set_position(2, 0 * gearRatio);
    break;
    case 1:
        picker.set_position(0, 60 * gearRatio);
        picker.set_position(1, 120 * gearRatio);
        picker.set_position(2, 180 * gearRatio);
    break;
    default:
    break;
    }
    picker.c620_read();
    picker.c620_calc();
}
int16_t LagoriPicker::getCurrent(int motor_id){
    picker.c620_read();
    return picker.read_current[motor_id];
}
void LagoriPicker::setVelocity(int motor_id, int speed){
    picker.set_velocity(motor_id, speed * 250);
    picker.c620_calc();
}