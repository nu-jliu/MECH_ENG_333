#ifndef UTILITIES_H__
#define UTILITIES_H__

#include <stdbool.h>

enum mode_t
{
    IDLE,
    PWM,
    ITEST,
    HOLD,
    TRACK
};

extern volatile enum mode_t mode;

extern volatile int ref_curr;

extern volatile unsigned int speed;
extern volatile int pwm_curr;
extern volatile bool direction;
extern volatile int count;
extern volatile int total_err;

extern volatile float kp_curr;
extern volatile float ki_curr;
extern volatile float kd_curr;

extern volatile float kp_pos;
extern volatile float ki_pos;
extern volatile float kd_pos;

void print_welcome_letter();
void set_motor_speed(int);
void printf_serial(const char *, ...);

#endif