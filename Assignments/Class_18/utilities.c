#include <string.h>
#include <stdarg.h>

#include "nu32dip.h"
#include "utilities.h"

#define BUFF_SIZE 256

volatile enum mode_t mode;

volatile int ref_curr;

volatile unsigned int speed;
volatile int pwm_curr;
volatile bool direction;
volatile int count;
volatile int total_err;

volatile float kp_curr;
volatile float ki_curr;
volatile float kd_curr;

volatile float kp_pos;
volatile float ki_pos;
volatile float kd_pos;

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