#include <xc.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

#include "nu32dip.h"
#include "encoder.h"
#include "i2c_master_noint.h"
#include "ina219.h"
#include "utilities.h"

#define BUFF_SIZE 256                // Bytes
#define PWM_FREQ 20000               // Hz
#define CURRENT_CONTROL_FREQ 5000    // Hz
#define POSITION_CONTROL_FREQ 200000 // Hz

void __ISR(_TIMER_3_VECTOR, IPL4SOFT) Current_Controller(void)
{
    // LATBbits.LATB14 = !LATBbits.LATB14;
    // // OC1RS = (unsigned int)((PR2 + 1) / 4);

    float curr;
    float error;
    float control_pwm;
    switch (mode)
    {
    case IDLE:
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

            pwm_curr += control_pwm;

            if (pwm_curr >= 100)
            {
                pwm_curr = 100;
            }
            else if (pwm_curr <= -100)
            {
                pwm_curr = -100;
            }

            set_motor_speed(pwm_curr);
            LATBbits.LATB14 = direction;
            OC1RS = (unsigned int)((float)(PR3 + 1) * speed / 100);
            break;
        }
        break;

    default:
        break;
    }

    IFS0bits.T3IF = 0;
}

int main(int argc, char **argv)
{
    char buffer[BUFF_SIZE];
    char mode_str[BUFF_SIZE];
    float kp, ki, kd;
    mode = IDLE;
    count = 0;
    total_err = 0;
    ref_curr = 200;

    NU32DIP_Startup();
    UART2_Startup();
    INA219_Startup();

    NU32DIP_YELLOW = 1;
    // NU32DIP_GREEN = 1;

    TRISBbits.TRISB14 = 0;
    LATBbits.LATB14 = 0;

    __builtin_disable_interrupts();

    // Set up PWM Signal
    T2CONbits.TCKPS = 0b000;               // prescalar of Timer2 of 1
    T2CONbits.TCS = 0;                     // Internal clock as clock source
    RPB15Rbits.RPB15R = 0b0101;            // remap B15 as OC1
    PR2 = NU32DIP_SYS_FREQ / PWM_FREQ - 1; // set PWM frequency as 20kHz
    TMR2 = 0;                              // initialize Timer2
    OC1CONbits.OCM = 0b110;                // PWM mode fault pin disables
    OC1CONbits.OC32 = 0;                   // 16bit compare
    OC1CONbits.OCTSEL = 0;                 // Timer2 as source
    OC1RS = 1800;                          // 75% duty cycle
    OC1R = 1800;                           // 75% duty cycle
    T2CONbits.ON = 1;                      // Enable Timer2
    OC1CONbits.ON = 1;                     // Enable OC1

    // Setup Current Controller
    PR3 = NU32DIP_SYS_FREQ / CURRENT_CONTROL_FREQ - 1;
    TMR3 = 0;
    T3CONbits.TCKPS = 0b000;
    T3CONbits.TGATE = 0;
    T3CONbits.TCS = 0;
    T3CONbits.ON = 1;
    IPC3bits.T3IP = 4;
    IPC3bits.T3IS = 0;
    IFS0bits.T3IF = 0;
    IEC0bits.T3IE = 1;

    __builtin_enable_interrupts();

    while (true)
    {
        // print_welcome_letter();
        // memset(buffer, '\0', sizeof(char) * BUFF_SIZE);
        // sprintf(buffer, "ENTER COMMAND: ");
        // NU32DIP_WriteUART1(buffer);
        memset(buffer, '\0', sizeof(char) * BUFF_SIZE);
        NU32DIP_ReadUART1(buffer, BUFF_SIZE);
        NU32DIP_YELLOW = 1;
        float current;

        switch (buffer[0])
        {
        case 'b':
            current = INA219_read_current();
            // memset(buffer, '\0', sizeof(char) * BUFF_SIZE);
            // sprintf(buffer, "\r\nThe motor current is %f mA.\r\n", current);
            // NU32DIP_WriteUART1(buffer);
            printf_serial("%f\r\n");
            break;

        case 'c':
            WriteUART2("a");
            while (!get_encoder_flag())
            {
                ;
            }
            set_encoder_flag(0);

            int p = get_encoder_count();

            // memset(buffer, '\0', 50);
            // sprintf(buffer, "\r\nThe motor angle is %d counts.\r\n", p);
            // NU32DIP_WriteUART1(buffer);
            printf_serial("%d\r\n", p);
            break;

        case 'd':
            WriteUART2("a");
            while (!get_encoder_flag())
            {
                ;
            }
            set_encoder_flag(0);

            int pos = get_encoder_count();
            float deg = (float)pos / 17367 * 360;

            // memset(buffer, '\0', 50);
            // sprintf(buffer, "\r\nThe motor angle is %f deg.\r\n", deg);
            // NU32DIP_WriteUART1(buffer);
            printf_serial("%f\r\n", deg);
            break;

        case 'e':
            break;

        case 'f':
            // memset(buffer, '\0', sizeof(char) * BUFF_SIZE);
            // sprintf(buffer, "\r\nWhat PWM value would you like [-100, 100]? ");
            // NU32DIP_WriteUART1(buffer);
            memset(buffer, '\0', sizeof(char) * BUFF_SIZE);
            NU32DIP_ReadUART1(buffer, sizeof(char) * BUFF_SIZE);
            int pwm;
            sscanf(buffer, "%d", &pwm);
            set_motor_speed(pwm);
            // if (speed >= 0)
            // {
            //     // direction = false;
            //     memset(buffer, '\0', sizeof(char) * BUFF_SIZE);
            //     sprintf(buffer, "\r\nPWM has been set to %d%% in the clockwise direction.\r\n", pwm);
            // }
            // else
            // {
            //     // direction = true;
            //     // speed = -speed;
            //     memset(buffer, '\0', sizeof(char) * BUFF_SIZE);
            //     sprintf(buffer, "\r\nPWM has been set to %d%% in the counterclockwise direction.\r\n", speed);
            // }
            printf_serial("%d\r\n", pwm_curr);
            // NU32DIP_WriteUART1(buffer);
            mode = PWM;
            break;

        case 'g':
            // memset(buffer, '\0', sizeof(char) * BUFF_SIZE);
            // sprintf(buffer, "\r\nEnter your desired Kp gain [recommended: 4.76]: ");
            // NU32DIP_WriteUART1(buffer);

            memset(buffer, '\0', sizeof(char) * BUFF_SIZE);
            NU32DIP_ReadUART1(buffer, sizeof(char) * BUFF_SIZE);
            sscanf(buffer, "%f", &kp);

            // memset(buffer, '\0', sizeof(char) * BUFF_SIZE);
            // sprintf(buffer, "\r\nEnter your desired Ki gain [recommended: 0.32]: ");
            // NU32DIP_WriteUART1(buffer);

            memset(buffer, '\0', sizeof(char) * BUFF_SIZE);
            NU32DIP_ReadUART1(buffer, sizeof(char) * BUFF_SIZE);
            sscanf(buffer, "%f", &ki);

            __builtin_disable_interrupts();

            kp_curr = kp;
            ki_curr = ki;

            __builtin_enable_interrupts();
            break;

        case 'h':
            // memset(buffer, '\0', sizeof(char) * BUFF_SIZE);
            // sprintf(buffer, "\r\nThe current controller is using Kp = %f, Ki = %f\r\n", kp_curr, ki_curr);
            // NU32DIP_WriteUART1(buffer);
            printf_serial("%f\r\n", kp_curr);
            printf_serial("%f\r\n", ki_curr);
            break;

        case 'i':
            memset(buffer, '\0', sizeof(char) * BUFF_SIZE);
            NU32DIP_ReadUART1(buffer, sizeof(char) * BUFF_SIZE);
            sscanf(buffer, "%f", &kp);

            memset(buffer, '\0', sizeof(char) * BUFF_SIZE);
            NU32DIP_ReadUART1(buffer, sizeof(char) * BUFF_SIZE);
            sscanf(buffer, "%f", &ki);

            memset(buffer, '\0', sizeof(char) * BUFF_SIZE);
            NU32DIP_ReadUART1(buffer, sizeof(char) * BUFF_SIZE);
            sscanf(buffer, "%f", &kd);

            __builtin_disable_interrupts();

            kp_pos = kp;
            ki_pos = ki;
            kd_pos = kd;

            __builtin_enable_interrupts();
            break;

        case 'j':
            printf_serial("%f\r\n", kp_pos);
            printf_serial("%f\r\n", ki_pos);
            printf_serial("%f\r\n", kd_pos);
            break;

        case 'k':
            mode = ITEST;
            break;

        case 'q':
            // memset(buffer, '\0', sizeof(char) * BUFF_SIZE);
            // sprintf(buffer, "\r\nExit Program ...\r\n");
            // NU32DIP_WriteUART1(buffer);
            return EXIT_SUCCESS;
            break;

        case 'p':
            mode = IDLE;
            break;

        case 'r':
            switch (mode)
            {
            case IDLE:
                memset(buffer, '\0', sizeof(char) * BUFF_SIZE);
                sprintf(mode_str, "IDLE");
                break;

            case PWM:
                memset(buffer, '\0', sizeof(char) * BUFF_SIZE);
                sprintf(mode_str, "PWM");
                break;

            case ITEST:
                memset(buffer, '\0', sizeof(char) * BUFF_SIZE);
                sprintf(mode_str, "ITEST");
                break;

            default:
                break;
            }

            memset(buffer, '\0', sizeof(char) * BUFF_SIZE);
            sprintf(buffer, "\r\nThe PIC32 controller is currently %s\r\n", mode_str);
            NU32DIP_WriteUART1(buffer);
            break;

        default:
            NU32DIP_YELLOW = 0;
            break;
        }

        // memset(buffer, '\0', sizeof(char) * BUFF_SIZE);
        // sprintf(buffer, "\r\n");
        // NU32DIP_WriteUART1(buffer);
    }

    return EXIT_SUCCESS;
}