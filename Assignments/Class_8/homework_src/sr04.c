
#include "sr04.h"
#include <xc.h>
#include <unistd.h>

#define MAX_TIME 100000000
#define PIC32_TIMER_FREQUENCY 24000000

// macros for the pins
#define TRIG LATBbits.LATB15
#define ECHO PORTBbits.RB14
// initialize the pins used by the SR04
void SR04_Startup()
{
    ANSELA = 0; // turn off the analog input functionality that overrides everything else
    ANSELB = 0;
    TRISBbits.TRISB15 = 0; // B15 is TRIG, output from the PIC
    TRISBbits.TRISB14 = 1; // B14 is ECHO, input to the PIC
    TRIG = 0;              // initialize TRIG to LOW
}
// trigger the SR04 and return the value in core timer ticks
unsigned int SR04_read_raw(unsigned int timeout)
{
    // turn on TRIG
    TRIG = 1;
    // wait at least 10us
    int time = _CP0_GET_COUNT();
    while (_CP0_GET_COUNT() - time < 240)
        ;
    // turn off TRIG
    TRIG = 0;
    // wait until ECHO is on
    while (ECHO == 0)
        ;
    // note the value of the core timer
    unsigned int time_start = _CP0_GET_COUNT();
    // wait until ECHO is off or timeout core ticks has passed
    while (ECHO == 1 && (_CP0_GET_COUNT() - time_start) < timeout)
        ;
    // note the core timer
    unsigned int time_end = _CP0_GET_COUNT();
    // return the difference in core times
    return time_end - time_start;
}
float SR04_read_meters()
{
    // read the raw rs04 value
    // convert the time to meters, the velocity of sound in air is 346 m/s
    // return the distance in meters
    unsigned int time_val = SR04_read_raw(24000000);
    float time_oneway = (float)time_val / 2.0;
    float time_second = time_oneway / PIC32_TIMER_FREQUENCY;
    return time_second * 346.0;
}
