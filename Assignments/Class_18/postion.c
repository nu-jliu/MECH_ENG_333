#include <xc.h>
#include <sys/attribs.h>

#include "position.h"
#include "utilities.h"
#include "nu32dip.h"

volatile float kp_pos;
volatile float ki_pos;
volatile float kd_pos;

volatile int index;
volatile int traj_len;

void __ISR(_TIMER_4_VECTOR, IPL5SOFT) Position_Controller(void)
{
    // NU32DIP_GREEN = !NU32DIP_GREEN;
    switch (mode)
    {
    case HOLD:
        int pos = read_encoder_count();
        int err = pos - pos_ref;
        total_err += err;
        int diff_err = prev_err - err;
        int pwm_control = (int)(kp_pos * err + kd_pos * diff_err + ki_pos * total_err);
        prev_err = err;

        if (pwm_control >= 100)
            pwm_control = 100;
        else if (pwm_control <= -100)
            pwm_control = -100;

        set_motor_speed(pwm_control);

        LATBbits.LATB14 = direction;
        OC1RS = (unsigned int)((float)(PR3 + 1) * speed / 100);
        break;

    case TRACK:
        break;

    default:
        break;
    }
    // if (mode == HOLD)
    // {
    //     int pos = read_encoder_count();
    //     int err = pos - pos_ref;
    //     total_err += err;
    //     int diff_err = prev_err - err;
    //     int pwm_control = (int)(kp_pos * err + kd_pos * diff_err + ki_pos * total_err);
    //     prev_err = err;

    //     if (pwm_control >= 100)
    //         pwm_control = 100;
    //     else if (pwm_control <= -100)
    //         pwm_control = -100;

    //     set_motor_speed(pwm_control);

    //     LATBbits.LATB14 = direction;
    //     OC1RS = (unsigned int)((float)(PR3 + 1) * speed / 100);

    //     // printf_serial("error: %d\r\n", err);
    //     // printf_serial("PWM: %d\r\n", pwm_control);
    // }

    IFS0bits.T4IF = 0;
}
