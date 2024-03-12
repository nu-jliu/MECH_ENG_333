#include <xc.h>
#include <sys/attribs.h>

#include "nu32dip.h"
#include "utilities.h"
#include "ina219.h"

volatile float kp_curr;
volatile float ki_curr;
volatile float kd_curr;

void __ISR(_TIMER_3_VECTOR, IPL4SOFT) Current_Controller(void)
{
    // LATBbits.LATB14 = !LATBbits.LATB14;
    // // OC1RS = (unsigned int)((PR2 + 1) / 4);

    int curr;
    float error;
    float control_pwm;
    int new_pwm;
    switch (mode)
    {
    case IDLE:
        pwm_curr = 0;
        OC1RS = 0;
        break;

    case PWM:
        LATBbits.LATB14 = direction;
        OC1RS = (unsigned int)((float)(PR3 + 1) * speed / 100);
        break;

    case ITEST:
        switch (++count)
        {
        case 100:
            mode = IDLE;
            count = 0;
            break;

        case 25:
        case 50:
        case 75:
            ref_curr = -ref_curr;

        default:
            curr = INA219_read_current();
            error = ref_curr - curr;
            total_err += error;
            control_pwm = kp_curr * error + ki_curr * total_err;

            new_pwm = pwm_curr + control_pwm;

            if (new_pwm >= 100)
                new_pwm = 100;
            else if (new_pwm <= -100)
                new_pwm = -100;

            set_motor_speed(new_pwm);
            LATBbits.LATB14 = direction;
            OC1RS = (unsigned int)((float)(PR3 + 1) * speed / 100);
            printf_serial("%d %d %d\r\n", ref_curr, curr, new_pwm);
            break;
        }
        break;

    default:
        break;
    }

    IFS0bits.T3IF = 0;
}