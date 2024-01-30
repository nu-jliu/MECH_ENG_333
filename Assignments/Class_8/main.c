#include <stdbool.h>
#include <string.h>
#include <xc.h>

#include "sr04.h"
#include "nu32dip.h"

#define BUFF_SIZE 256
#define HALF_FREQENCY 24000000

void delay(int ms)
{
    int time = _CP0_GET_COUNT();

    while (_CP0_GET_COUNT() - time < HALF_FREQENCY / 1000 * ms)
        ;
}

int main(void)
{
    char buff[BUFF_SIZE];
    memset(buff, '\0', BUFF_SIZE * sizeof(char));

    NU32DIP_Startup();
    SR04_Startup();
    while (true)
    {
        float distance = SR04_read_meters();
        // float distance = 0.5;
        memset(buff, '\0', BUFF_SIZE * sizeof(char));
        sprintf(buff, "SR04 value in meters: %f\r\n", distance);
        NU32DIP_WriteUART1(buff);

        NU32DIP_GREEN = !NU32DIP_GREEN;
        NU32DIP_YELLOW = !NU32DIP_YELLOW;

        delay(250);
    }
}