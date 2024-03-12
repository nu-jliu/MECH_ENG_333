#include <string.h>
#include <stdarg.h>

#include "nu32dip.h"
#include "utilities.h"
#include "encoder.h"

#define BUFF_SIZE 256
#define ENCODER_COUNTS_PER_REV 384

volatile enum mode_t mode;

volatile int ref_curr;
volatile int pos_ref;

volatile unsigned int speed;
volatile int pwm_curr;
volatile bool direction;
volatile int count;
volatile int total_err;
volatile int prev_err;

void printf_serial(const char *format, ...)
{
    char buff[BUFF_SIZE];
    memset(buff, '\0', BUFF_SIZE * sizeof(char));

    va_list args;
    va_start(args, format);

    vsprintf(buff, format, args);
    NU32DIP_WriteUART1(buff);
}

void print_welcome_letter()
{
    char buff[BUFF_SIZE];

    // memset(buff, '\0', sizeof(char) * BUFF_SIZE);
    // sprintf(buff, "PIC32 MOTOR DRIVER INTERFACE\r\n\r\n");
    // NU32DIP_WriteUART1(buff);
    printf_serial("PIC32 MOTOR DRIVER INTERFACE\r\n\r\n");
    memset(buff, '\0', sizeof(char) * BUFF_SIZE);
    sprintf(buff, "\ta: Read current sensor (ADC counts)\tb: Read current sensor (mA)\r\n");
    NU32DIP_WriteUART1(buff);
    memset(buff, '\0', sizeof(char) * BUFF_SIZE);
    sprintf(buff, "\tc: Read encoder (counts)\t\td: Read encoder (deg)\r\n");
    NU32DIP_WriteUART1(buff);
    memset(buff, '\0', sizeof(char) * BUFF_SIZE);
    sprintf(buff, "\te: Reset encoder\t\t\tf: Set PWM (-100 to 100)\r\n");
    NU32DIP_WriteUART1(buff);
    memset(buff, '\0', sizeof(char) * BUFF_SIZE);
    sprintf(buff, "\tg: Set current gains\t\t\th: Get current gains\r\n");
    NU32DIP_WriteUART1(buff);

    memset(buff, '\0', sizeof(char) * BUFF_SIZE);
    sprintf(buff, "\tk: Test current control\t\t\tl: Go to angle (deg)\r\n");
    NU32DIP_WriteUART1(buff);

    memset(buff, '\0', sizeof(char) * BUFF_SIZE);
    sprintf(buff, "\r\n\r\n");
    NU32DIP_WriteUART1(buff);
}

void set_motor_speed(int pwm)
{
    pwm_curr = pwm;

    if (pwm >= 0)
    {
        speed = pwm;
        direction = false;
        // memset(buffer, '\0', sizeof(char) * BUFF_SIZE);
        // sprintf(buffer, "PWM has been set to %d%% in the clockwise direction.", pwm);
    }
    else
    {
        speed = -pwm;
        direction = true;
        // memset(buffer, '\0', sizeof(char) * BUFF_SIZE);
        // sprintf(buffer, "PWM has been set to %d%% in the counterclockwise direction.", pwm);
    }
}

int read_encoder_count()
{
    WriteUART2("a");
    while (!get_encoder_flag())
    {
        ;
    }
    set_encoder_flag(0);

    return get_encoder_count();
}

float count2deg(int count)
{
    return (float)count / (float)ENCODER_COUNTS_PER_REV * 360.0;
}

int deg2count(float degree)
{
    return (int)(degree * ENCODER_COUNTS_PER_REV / 360);
}