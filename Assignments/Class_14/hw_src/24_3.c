#include <stdbool.h>
#include "nu32dip.h"

// ADC information
#define MAXPLOTPTS 1000
#define SAMPLE_TIME 6 // 24MHz*250ns
#define NUMSAMPS 1000
void ADC_Startup();
unsigned int adc_sample_convert(int);
void makeWaveform();

static volatile int Waveform[NUMSAMPS];

void __ISR(_TIMER_2_VECTOR, IPL5SOFT) Controller(void)
{                           // _TIMER_2_VECTOR = 8
    static int counter = 0; // initialize counter once

    // insert line(s) to set OC1RS
    OC1RS = Waveform[counter];

    counter++; // add one to counter every time ISR is entered
    if (counter == NUMSAMPS)
    {
        counter = 0; // roll the counter over when needed
    }

    // insert line to clear interrupt flag
    IFS0bits.T2IF = 0;
}

int main(void)
{
    NU32DIP_Startup();
    ADC_Startup();

    makeWaveform();

    NU32DIP_GREEN = 1;

    char message[100];
    char command;
    int datapoints;
    int index;
    short data[MAXPLOTPTS];

    __builtin_disable_interrupts();

    PR2 = 48000;
    TMR2 = 0;
    T2CONbits.TCKPS = 0b000;
    T2CONbits.TGATE = 0;
    T3CONbits.TCS = 0;
    T2CONbits.ON = 1;
    IPC2bits.T2IP = 5;
    IPC2bits.T2IS = 0;
    IFS0bits.T2IF = 0;
    IEC0bits.T2IE = 1;

    __builtin_enable_interrupts();

    T3CONbits.TCKPS = 0b000;    // prescalar of 1
    RPB15Rbits.RPB15R = 0b0101; // B15 as OC1
    PR3 = 2399;                 // 20kHz
    TMR3 = 0;                   // initialize timer3
    OC1CONbits.OCM = 0b110;     // PWM mode fault pin disabled
    OC1CONbits.OC32 = 0;        // 16bit compare
    OC1CONbits.OCTSEL = 1;      // Timer3 as source
    OC1RS = 1800;               // 75% duty cycle
    OC1R = 1800;                // same as above
    T3CONbits.ON = 1;           // enable Timer3
    OC1CONbits.ON = 1;          // enable OC1

    while (true)
    {
        // wait for oscope.py to send a command and number of data points to collect
        NU32DIP_ReadUART1(message, 100);
        sscanf(message, "%c %d", &command, &datapoints);
        if (datapoints > MAXPLOTPTS)
        {
            datapoints = MAXPLOTPTS;
        }

        // get the data
        if (command == 'a')
        {
            // read at 100Hz
            for (index = 0; index < datapoints; index++)
            {
                _CP0_SET_COUNT(0);
                data[index] = adc_sample_convert(1);
                while (_CP0_GET_COUNT() < 24000000 / 100)
                {
                }
            }
        }
        else if (command == 'b')
        {
            // read at 200kHz
            for (index = 0; index < datapoints; index++)
            {
                _CP0_SET_COUNT(0);
                data[index] = adc_sample_convert(1);
                while (_CP0_GET_COUNT() < 24000000 / 200000)
                {
                }
            }
        }

        // print the data back
        for (index = 0; index < datapoints; index++)
        {
            sprintf(message, "%d %d\r\n", datapoints - index, data[index]);
            NU32DIP_WriteUART1(message);
        }
    }
    return 0;
}

void ADC_Startup()
{
    ANSELAbits.ANSA1 = 1; // AN1 is an adc pin
    AD1CON3bits.ADCS = 1; // ADC clock period is Tad = 2*(ADCS+1)*Tpb =2*2*(1/48000000Hz) = 83ns > 75ns
    AD1CON1bits.ADON = 1;
}

unsigned int adc_sample_convert(int pin)
{
    unsigned int elapsed = 0, finish_time = 0;
    AD1CHSbits.CH0SA = pin;
    AD1CON1bits.SAMP = 1;
    elapsed = _CP0_GET_COUNT();
    finish_time = elapsed + SAMPLE_TIME;
    while (_CP0_GET_COUNT() < finish_time)
    {
        ;
    }
    AD1CON1bits.SAMP = 0;
    while (!AD1CON1bits.DONE)
    {
        ;
    }
    return ADC1BUF0;
}

void makeWaveform()
{
    int i = 0, center = 1200, A = 600; // square wave, fill in center value and amplitude
    for (i = 0; i < NUMSAMPS; ++i)
    {
        if (i < NUMSAMPS / 2)
        {
            Waveform[i] = center + A;
        }
        else
        {
            Waveform[i] = center - A;
        }
    }
}