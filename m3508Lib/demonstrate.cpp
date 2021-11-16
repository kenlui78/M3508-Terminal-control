#include "m3508.h"
#include "mbed.h"
#include <cstdio>

CAN can1(PA_11, PA_12, 1000000);
m3508 _m3508;

DigitalIn upbutton(PC_13); // User Button
DigitalOut led(LED1);
bool state = false;

//steps and delay counter
Timer t2;
int step = 0;
int counter = 0;

int main() {
  _m3508.m3508_init(&can1);

  // set the PID term of the 3 Ring PID loop
  // use c610 +2006 as example
  // suggest velocity mode setting 
  //_m3508.set_i_pid_param(0, 1.7, 0.0, 0.000005); // Torque PID W1
  //_m3508.set_v_pid_param(0, 15.4, 0.01, 0.005);  // Velocity PID W1
  //_m3508.set_p_pid_param(0, 9.5, 0.01, 0.3);     // position PID W1
  
  //suggest position mode setting 
  _m3508.set_i_pid_param(0, 1.5, 0.001, 0.000005); // Torque PID W1
  _m3508.set_v_pid_param(0, 5.5, 0.001, 0.005);  // Velocity PID W1
  _m3508.set_p_pid_param(0, 12.5, 0.4, 0.35);     // position PID W1
  
  // set the LP filter of the desire control
  _m3508.profile_velocity_CCW[0] = 12000;//Maximum is 12000 for c620 and 10000 for c610
  _m3508.profile_velocity_CW[0] = -12000;
  _m3508.profile_torque_CCW[0] = 16000; //Maximum is 16000 for c620 and 10000 for c610
  _m3508.profile_torque_CW[0] = -16000;
  // set the current limit, overcurrent may destory the driver
  _m3508.motor_max_current = 16000; // 10000 max for c610+2006 16000 max for c620
  
  // output position = motor rotation(deg) * gear ratio.  2006 = 1:36 , 3508
  // = 1:19 rotate 180 deg
  _m3508.set_position(0, 0);// set starting position as 0
  //_m3508.set_velocity(0, 0);
  while (true) {
    if (!upbutton) {
      state = !state;
    }
    if (state) {
      led = 1;
      t2.start();
      counter++;
      //delay 1 sec
      if (counter >= 1002) {
        counter = 0;
        t2.stop();
        step++;
        t2.reset();
      }
    } else {
      led = 0;
      t2.stop();
      step = 0;
    }
    switch (step) {
    case 1:
      _m3508.set_position(0, 60 * 19);
      //_m3508.set_velocity(0, 1000);
      break;
    case 2:
      _m3508.set_position(0, 120 * 19);
      //_m3508.set_velocity(0, 2000);
      break;
    case 3:
      _m3508.set_position(0, 180 * 19);
      //_m3508.set_velocity(0, 4000);
      break;
    case 4:
      _m3508.set_position(0, 240 * 19);
        //_m3508.set_velocity(0, 6000);
      break;
    case 5:
      _m3508.set_position(0, 300 * 19);
      //_m3508.set_velocity(0, 8000);
      break;
    case 6:
      _m3508.set_position(0, 360 * 19);
      //_m3508.set_velocity(0, 10000);
      break;
    case 7:
      _m3508.set_position(0, 300 * 19);
      //_m3508.set_velocity(0, -10000);
      break;
    case 8:
      _m3508.set_position(0, 240 * 19);
      //_m3508.set_velocity(0, -8000);
      break;
    case 9:
      _m3508.set_position(0, 180 * 19);
      //_m3508.set_velocity(0, -7000);
      break;
    case 10:
      _m3508.set_position(0, 120 * 19);
      //_m3508.set_velocity(0, -6000);
      break;
    case 11:
      _m3508.set_position(0, 60 * 19);
      //_m3508.set_velocity(0, -5000);
      break;
    case 12:
      _m3508.set_position(0, 0);
      //_m3508.set_velocity(0, 0);
      step = 0;
      break;
    default:
      _m3508.set_position(0, 0);
      break;
    }
    // need to run in order to keep the PID running
    _m3508.c620_read();
    _m3508.c620_calc();
    // delay 1 millisec = 1000Hz = c610/c620 can bus feedback frequency
    ThisThread::sleep_for(1ms);
  }
}
