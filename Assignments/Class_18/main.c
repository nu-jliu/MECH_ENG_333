#include <xc.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

#include "nu32dip.h"
#include "encoder.h"
#include "i2c_master_noint.h"
#include "ina219.h"
#include "utilities.h"
#include "current.h"
#include "position.h"

#define BUFF_SIZE 256             // Bytes
#define PWM_FREQ 20000            // Hz
#define CURRENT_CONTROL_FREQ 5000 // Hz
#define POSITION_CONTROL_FREQ 200 // Hz
#define MAX_POINTS 100

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
    NU32DIP_GREEN = 0;

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
    PR3 = NU32DIP_SYS_FREQ / 4 / CURRENT_CONTROL_FREQ - 1; // Set frequency of 5kHz
    TMR3 = 0;                                              // Initialize Timer 3
    T3CONbits.TCKPS = 0b010;                               // Set prescaler of Timer 3 as 1:4
    T3CONbits.TGATE = 0;                                   // Disable gated accumulation
    T3CONbits.TCS = 0;                                     // Internal peripheral clock
    T3CONbits.ON = 1;                                      // Enable Timer 3
    IPC3bits.T3IP = 4;                                     // Priority as 4
    IPC3bits.T3IS = 0;                                     // Sub-priority of 0
    IFS0bits.T3IF = 0;                                     // Clear Timer 4 interrupt flag
    IEC0bits.T3IE = 1;                                     // Enable Timer 4 interrupt

    // Setup current controller
    PR4 = NU32DIP_SYS_FREQ / POSITION_CONTROL_FREQ - 1; // Set frequenct of 200kHz
    TMR4 = 0;                                           // Initialize Timer 4
    T4CONbits.TCKPS = 0b000;                            // Set prescaler of Timer 4 as 1
    T4CONbits.TGATE = 0;                                // Disable the gated accumulation
    T4CONbits.TCS = 0;                                  // Internal peripheral clock
    T4CONbits.ON = 1;                                   // Enable Timer 4
    IPC4bits.T4IP = 5;                                  // Iterrupt priority of 5
    IPC4bits.T4IS = 0;                                  // Sub-priority of 0
    IFS0bits.T4IF = 0;                                  // Interrupt flag of 0
    IEC0bits.T4IE = 1;                                  // Enable interrupt

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
        int pos;

        switch (buffer[0])
        {
        case 'b':;
            float current = INA219_read_current();
            // memset(buffer, '\0', sizeof(char) * BUFF_SIZE);
            // sprintf(buffer, "\r\nThe motor current is %f mA.\r\n", current);
            // NU32DIP_WriteUART1(buffer);
            printf_serial("%f\r\n");
            break;

        case 'c':
            pos = read_encoder_count();

            printf_serial("%d\r\n", pos);
            break;

        case 'd':
            pos = read_encoder_count();
            float deg = count2deg(pos);

            printf_serial("%f\r\n", deg);
            break;

        case 'e':
            WriteUART2("b");
            break;

        case 'f':
            memset(buffer, '\0', sizeof(char) * BUFF_SIZE);
            NU32DIP_ReadUART1(buffer, sizeof(char) * BUFF_SIZE);
            int pwm;
            sscanf(buffer, "%d", &pwm);
            set_motor_speed(pwm);
            printf_serial("%d\r\n", pwm_curr);

            __builtin_disable_interrupts();

            mode = PWM;

            __builtin_enable_interrupts();
            break;

        case 'g':
            memset(buffer, '\0', sizeof(char) * BUFF_SIZE);
            NU32DIP_ReadUART1(buffer, sizeof(char) * BUFF_SIZE);
            sscanf(buffer, "%f", &kp);

            memset(buffer, '\0', sizeof(char) * BUFF_SIZE);
            NU32DIP_ReadUART1(buffer, sizeof(char) * BUFF_SIZE);
            sscanf(buffer, "%f", &ki);

            __builtin_disable_interrupts();

            kp_curr = kp;
            ki_curr = ki;

            __builtin_enable_interrupts();
            break;

        case 'h':
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
            printf_serial("%d\r\n", MAX_POINTS - 1);

            __builtin_disable_interrupts();

            total_err = 0;
            mode = ITEST;

            __builtin_disable_interrupts();
            break;

        case 'l':
            memset(buffer, '\0', sizeof(char) * BUFF_SIZE);
            NU32DIP_ReadUART1(buffer, sizeof(char) * BUFF_SIZE);

            float desired_deg;
            int desired_count;
            sscanf(buffer, "%f", &desired_deg);

            desired_count = deg2count(desired_deg);

            __builtin_disable_interrupts();

            pos_ref = desired_count;
            mode = HOLD;

            __builtin_enable_interrupts();
            break;

        case 'p':
            __builtin_disable_interrupts();

            mode = IDLE;

            __builtin_enable_interrupts();
            break;

        case 'q':
            NU32DIP_GREEN = 1;
            __builtin_disable_interrupts();
            return EXIT_SUCCESS;
            break;

        case 'r':
            switch (mode)
            {
            case IDLE:
                printf_serial("IDLE\r\n");
                break;

            case PWM:
                printf_serial("PWM\r\n");
                break;

            case ITEST:
                printf_serial("ITEST\r\n");
                break;

            case HOLD:
                printf_serial("HOLD\r\n");
                break;

            case TRACK:
                printf_serial("TRACK\r\n");
                break;

            default:
                break;
            }

            break;

        default:
            NU32DIP_YELLOW = 0;
            break;
        }
    }

    return EXIT_SUCCESS;
}