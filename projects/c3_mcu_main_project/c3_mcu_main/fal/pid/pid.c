
#include "pid.h"


struct _pid pid[2];


float PID_Speed_Incr(struct _pid *pid, float curSpeed)
{
        //curSpeed为当前实际速度 SetSpeed为期望速度
        pid->err = pid->SetSpeed - curSpeed;
        //增量式pid算法公式
        pid->increment = pid->Kp * (pid->err - pid->err_next) + 
                         pid->Ki * pid->err + 
                         pid->Kd * (pid->err - 2 * pid->err_next + pid->err_last);
        //判断增量是否超过最大值最小值
        pid->increment = pid->increment >pid->add_max?pid->add_max:pid->increment;
        pid->increment = pid->increment <pid->add_min?pid->add_min:pid->increment;
        //累加
        pid->output += pid->increment;
        //判断输出量是否超过最大值最小值
        pid->output = pid->output > pid->umax?pid->umax:pid->output;
        pid->output = pid->output < pid->umin?pid->umin:pid->output;
        //误差传递
        pid->err_last = pid->err_next;
        pid->err_next = pid->err;
        //输出值在这套系统中的表现形式为电机pwm
        return pid->output;
}
void PID_Init(struct _pid *pid)
{
        pid->SetSpeed = 0.0;
        pid->err = 0.0;
        pid->err_last = 0.0;
        pid->err_next = 0.0;
        pid->voltage = 0.0;
        pid->integral = 0.0;
        pid->Kp = 0.001;
        pid->output =0;
        pid->Ki = 0.0005;
        pid->Kd = 0.0;
        pid->umax = 0.99;
        pid->umin = 0;
        pid->increment = 0.0;
        pid->add_min = -0.1;
        pid->add_max =0.2;
        pid->limit_error = 1.0;         // 1cm/s 允许误差1cm/s
}




