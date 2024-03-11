#ifndef UTILITIES__H__
#define UTILITIES__H__

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
extern volatile int pos_ref;

extern volatile unsigned int speed;
extern volatile int pwm_curr;
extern volatile bool direction;
extern volatile int count;
extern volatile int total_err;
extern volatile int prev_err;

void print_welcome_letter(void);
void set_motor_speed(int);
void printf_serial(const char *, ...);
int read_encoder_count(void);
float count2deg(int);
int deg2count(float);

#endif // UTILITIES__H__