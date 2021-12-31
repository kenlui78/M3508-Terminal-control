#include "m3508.h"
#include "mbed.h"
#include <stdint.h>

/////User Define/////
#define constrain(amt, low, high)                                              \
  ((amt) < (low) ? (low) : ((amt) > (high) ? (high) : (amt)))
#ifdef UART_Debug
// Serial pc(USBTX, USBRX,115200);
#endif

typedef enum { m3508_vel_loop = 0, m3508_pos_loop } m3508_mode;

CANMessage receive_msg;

void m3508::m3508_init(CAN *_CAN) {
  can1 = _CAN;
  can1->frequency(1000000);
}

int16_t m3508::utils_truncate_number(int16_t number, int min, int max) {
  if (number > max) {
    number = max;
  } else if (number < min) {
    number = min;
  }
  return number;
}
int16_t m3508::utils_truncate_number_abs(int16_t number, int max) {
  if (number > max) {
    number = max;
  } else if (number < -max) {
    number = -max;
  }
  return number;
}

/////CAN send all motors/////
void m3508::CAN_Send(int16_t current1, int16_t current2, int16_t current3, int16_t current4,int16_t current5, int16_t current6, int16_t current7, int16_t current8)
//void m3508::CAN_Send(int16_t current1, int16_t current2, int16_t current3, int16_t current4)  
{
  CANMessage TxMessage;
  TxMessage.id = 0x200;           //Common ID of c620 
  TxMessage.format = CANStandard; //Standard Frame
  TxMessage.type = CANData;       //Data 
  TxMessage.len = 8;              // Size = 8 byte
  
  // Motor Current Value Range from -16384 to 16384
  // Motor 1 Current Control
  TxMessage.data[0] = current1 >> 8; // data1;
  TxMessage.data[1] = current1;      // data2;
  // Motor 2 Current Control
  TxMessage.data[2] = current2 >> 8; // data3;
  TxMessage.data[3] = current2;      // data4;
  // Motor 3 Current Control
  TxMessage.data[4] = current3 >> 8; // data5;
  TxMessage.data[5] = current3;      // data6;
  // Motor 4 Current Control
  TxMessage.data[6] = current4 >> 8; // data7;
  TxMessage.data[7] = current4;      // data8;
  
  CANMessage TxMessage2;
  TxMessage2.id = 0x1FF;           //Common ID of c620 
  TxMessage2.format = CANStandard; //Standard Frame
  TxMessage2.type = CANData;       //Data 
  TxMessage2.len = 8;              // Size = 8 byte
  
  // Motor Current Value Range from -16384 to 16384
  // Motor 5 Current Control
  TxMessage2.data[0] = current5 >> 8; // data1;
  TxMessage2.data[1] = current5;      // data2;
  // Motor 6 Current Control
  TxMessage2.data[2] = current6 >> 8; // data3;
  TxMessage2.data[3] = current6;      // data4;
  // Motor 7 Current Control
  TxMessage2.data[4] = current7 >> 8; // data5;
  TxMessage2.data[5] = current7;      // data6;
  // Motor 8 Current Control
  TxMessage2.data[6] = current8 >> 8; // data7;
  TxMessage2.data[7] = current8;      // data8;

  can1->write(TxMessage2);
  can1->write(TxMessage);
}

/////CAN Send is Triggered when Receiving CAN Message/////
void m3508::c620_read() // CAN
{
  can1->read(receive_msg);
  int16_t actual_local_position = (int16_t)(receive_msg.data[0] << 8) | receive_msg.data[1];
  int16_t actual_velocity = (int16_t)(receive_msg.data[2] << 8) | receive_msg.data[3];
  int16_t actual_current = (int16_t)(receive_msg.data[4] << 8) | receive_msg.data[5];
  int motor_index = 0x00;
  switch (receive_msg.id) {
  case Motor_1_RevID: {
    motor_index = 0;
    //printf("%d\n",motor_index);
    break;
  }
  case Motor_2_RevID: {
    motor_index = 1;
    break;
  }
  case Motor_3_RevID: {
    motor_index = 2;
    break;
  }
  case Motor_4_RevID: {
    motor_index = 3;
    break;
  }
  case Motor_5_RevID: {
    motor_index = 4;
    break;
  }
  case Motor_6_RevID: {
    motor_index = 5;
    break;
  }
  case Motor_7_RevID: {
    motor_index = 6;
    break;
  }
  case Motor_8_RevID: {
    motor_index = 7;
    break;
  }
  default:
    return;
  }

  read_velocity[motor_index] = actual_velocity;
  read_position[motor_index] = actual_local_position;
  read_current[motor_index] = actual_current;

  // To record the starting position
  if (pos_init[motor_index]) {
    start_pos[motor_index] = read_position[motor_index];
    pos_init[motor_index] = false;
  }
  // Accumulate the round count
  if (read_position[motor_index] - last_pos[motor_index] > 4192) {
    round_cnt[motor_index]--;
  } else if (read_position[motor_index] - last_pos[motor_index] < -4192) {
    round_cnt[motor_index]++;
  } else {
      //printf("%d\n",round_cnt[motor_index]);
  }

  // Multi-turn global position
  global_pos[motor_index] = (round_cnt[motor_index] * 8193 + read_position[motor_index]) - start_pos[motor_index];
  // Map to degree, 0-360
  global_angle[motor_index] = (global_pos[motor_index] * 360.0) / 8193.0;
  // After using the value read_position, assign its value to last_pos
  last_pos[motor_index] = read_position[motor_index]; // used for round count
}

void m3508::c620_calc() // Execution frequency 100Hz
{
  t_pid.stop();
  dt = (float)std::chrono::duration_cast<std::chrono::microseconds>(t_pid.elapsed_time()).count() / 1000000;
  for (int motor_index = 0; motor_index < 8; motor_index++) {
    // velocity loop
    if (loop_mode[motor_index] == m3508_vel_loop) {
      v_pid[motor_index].P = required_velocity[motor_index] - read_velocity[motor_index];
      v_pid[motor_index].I += v_pid[motor_index].P * dt;
      v_pid[motor_index].D = (v_pid[motor_index].P - v_pid[motor_index].prev_err) / dt;
      
      // Store old velocity error
      v_pid[motor_index].prev_err = v_pid[motor_index].P;

      // Calculate output current
      required_current[motor_index] = (int16_t)floor((v_pid[motor_index].P * v_pid[motor_index].kP + v_pid[motor_index].I * v_pid[motor_index].kI + v_pid[motor_index].D * v_pid[motor_index].kD));
      required_current[motor_index] = (int16_t)constrain(required_current[motor_index],profile_torque_CW[motor_index],profile_torque_CCW[motor_index]);

      // current loop
      //i_pid[motor_index].P = required_current[motor_index] - read_current[motor_index];
      //i_pid[motor_index].I += i_pid[motor_index].P * dt;
      //i_pid[motor_index].D = (i_pid[motor_index].P - i_pid[motor_index].prev_err) / dt;

      //Store the error      
      i_pid[motor_index].prev_err = i_pid[motor_index].P;

      // Calculate output current
      motor_out[motor_index] =  (int16_t)floor((i_pid[motor_index].P * i_pid[motor_index].kP + i_pid[motor_index].I * i_pid[motor_index].kI + i_pid[motor_index].D * i_pid[motor_index].kD));
    }
    //position mode
    else if (loop_mode[motor_index] == m3508_pos_loop) {
      // position loop
      p_pid[motor_index].P = required_position[motor_index] - global_angle[motor_index];
      p_pid[motor_index].I += p_pid[motor_index].P * dt;
      p_pid[motor_index].D = (p_pid[motor_index].P - p_pid[motor_index].prev_err) / dt;

      // Store previous error
      p_pid[motor_index].prev_err = p_pid[motor_index].P;

      // Sum up the terms -> require velocity 
      required_velocity[motor_index] = (int16_t)floor(p_pid[motor_index].kP * p_pid[motor_index].P + p_pid[motor_index].kI * p_pid[motor_index].I + p_pid[motor_index].kD * p_pid[motor_index].D);

      required_velocity[motor_index] = (int16_t)constrain(required_velocity[motor_index], profile_velocity_CW[motor_index], profile_velocity_CCW[motor_index]); //max 7200 for 2006

      // velocity loop
      v_pid[motor_index].P = required_velocity[motor_index] - read_velocity[motor_index];
      v_pid[motor_index].I += v_pid[motor_index].P * dt;
      v_pid[motor_index].D = (v_pid[motor_index].P - v_pid[motor_index].prev_err) / dt;
      
      // Store old velocity error
      v_pid[motor_index].prev_err = v_pid[motor_index].P;

      // Calculate output current
      required_current[motor_index] = (int16_t)floor((v_pid[motor_index].P * v_pid[motor_index].kP + v_pid[motor_index].I * v_pid[motor_index].kI + v_pid[motor_index].D * v_pid[motor_index].kD));
      required_current[motor_index] = (int16_t)constrain(required_current[motor_index],profile_torque_CW[motor_index],profile_torque_CCW[motor_index]);

      // current loop
      //i_pid[motor_index].P = required_current[motor_index] - read_current[motor_index];
      //i_pid[motor_index].I += i_pid[motor_index].P * dt;
      //i_pid[motor_index].D = (i_pid[motor_index].P - i_pid[motor_index].prev_err) / dt;

      //Store the error      
      i_pid[motor_index].prev_err = i_pid[motor_index].P;

      // Calculate output current
      motor_out[motor_index] =  (int16_t)floor((i_pid[motor_index].P * i_pid[motor_index].kP + i_pid[motor_index].I * i_pid[motor_index].kI + i_pid[motor_index].D * i_pid[motor_index].kD));
    }
  }

  // Set current constaint according to m3508 & c620 official manual (m3508
  // rated 10A, c620 rated 0 - 20A) Max 16384 -> 16384/2 = 8192 = 10 A, limit to
  // 7000 = 8.5A for testing
  for (int i = 0; i < 8; i++) {
    motor_out[i] = constrain(motor_out[i], -motor_max_current, motor_max_current);
  }
  CAN_Send(motor_out[0], motor_out[1], motor_out[2], motor_out[3], motor_out[4], motor_out[5], motor_out[6], motor_out[7]);
  //CAN_Send(motor_out[0], motor_out[1], motor_out[2], motor_out[3]);
  t_pid.reset();
  t_pid.start();
}

void m3508::set_velocity(int motor_index, int speed) {
  loop_mode[motor_index] = m3508_vel_loop;
  required_velocity[motor_index] = speed;
}
// pos loop input degree value (float)
void m3508::set_position(int motor_index, float pos) {
  loop_mode[motor_index] = m3508_pos_loop;
  required_position[motor_index] = pos;
}
void m3508::set_v_pid_param(int ID, float kp, float ki, float kd) {
  v_pid[ID].kP = kp;
  v_pid[ID].kI = ki;
  v_pid[ID].kD = kd;
}
void m3508::set_p_pid_param(int ID, float kp, float ki, float kd) {
  p_pid[ID].kP = kp;
  p_pid[ID].kI = ki;
  p_pid[ID].kD = kd;
}
void m3508::set_i_pid_param(int ID, float kp, float ki, float kd) {
  i_pid[ID].kP = kp;
  i_pid[ID].kI = ki;
  i_pid[ID].kD = kd;
}
