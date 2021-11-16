#include "mbed.h"
#include <cstdint>

#define Motor_1_RevID 0x201
#define Motor_2_RevID 0x202
#define Motor_3_RevID 0x203
#define Motor_4_RevID 0x204
#define Motor_5_RevID 0x205
#define Motor_6_RevID 0x206
#define Motor_7_RevID 0x207
#define Motor_8_RevID 0x208

struct pid{
    float err = 0;
    float P = 1;
    float I = 1;
    float D = 1;
    float kP = 0;
    float kI = 0;
    float kD = 0;
    float prev_err = 0;
    };

class m3508
{
    public:
        //Change to m3508_init to replace constructor      
        void m3508_init(CAN* _CAN);    
        CAN* can1;
        
        //From VESC
        int16_t utils_truncate_number(int16_t number, int min, int max);
        int16_t utils_truncate_number_abs(int16_t number, int max);
        
        //C620 function
        void c620_read();//The frequency needs to be at least 100Hz, or the motor will be too retard
        void c620_calc();

        void CAN_Send(int16_t current1, int16_t current2, int16_t current3, int16_t current4,int16_t current5, int16_t current6, int16_t current7, int16_t current8);
        //void CAN_Send(int16_t current1, int16_t current2, int16_t current3, int16_t current4);
        void set_velocity(int id, int speed);
        void set_position(int id, float pos);

        void set_mode(int mode);

        void set_v_pid_param(int ID, float kp, float ki, float kd);
        void set_p_pid_param(int ID, float kp, float ki, float kd);
        void set_i_pid_param(int ID, float kp, float ki, float kd);

        //Those three variables will be directly referenced in loop
        //Hence, modifying the values will change CAN output
        int16_t required_current[8] = {0};
        float required_position[8]   = {0};
        int16_t required_velocity[8] = {0};
        int16_t motor_out[8] = {0}; 
        
        //Read Value
        int16_t read_current[8]={0};
        uint16_t read_position[8]={0};//value 0-8192 for c620, 0-8191 for c610
        int16_t read_velocity[8]={0};//value: rpm
        
        //Position Value
        int round_cnt[8] = {0};
        int16_t last_pos[8] = {0};
        int start_pos [8] = {0};//The position of the first CAN message received, 
        bool pos_init[8] = {true};
        int global_pos[8] = {0};
        float global_angle[8] = {0};
        
        //mode
        int loop_mode[8];
        
        //pid parameter
        float dt = 0.0005;//Time period
        float dt_int[8] = {0};//dt integral
        float d_filter = 0.0;
        float p_pid_kd_filter = 0.9;

        pid v_pid[8];
        pid p_pid[8];
        pid i_pid[8];   
        Timer t_pid;

        //motor and driver parameters
        int16_t profile_velocity_CW[8] = {}; // 0 ~ +7200 max for c610
        int16_t  profile_velocity_CCW[8] = {};  //0~ -7200 max for c610
        int16_t  profile_torque_CW[8] = {}; // recomanded not over +50000
        int16_t profile_torque_CCW[8] = {};// recomanded not less than -50000
        int motor_max_current = 7000; // default
    };