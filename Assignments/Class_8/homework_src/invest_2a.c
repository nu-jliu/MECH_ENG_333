#include <stdio.h>
#include <string.h>
#include "nu32dip.h"

#define MAX_YEARS 100
#define BUFF_LEN 256

typedef struct
{
    double inv0;
    double growth;
    int years;
    double invarray[MAX_YEARS + 1];
} Investment;

int getUserInput(Investment *invp);
void calculateGrowth(Investment *invp);
void sendOutput(double *arr, int years);

int main(void)
{
    Investment inv;

    NU32DIP_Startup();
    while (getUserInput(&inv))
    {
        inv.invarray[0] = inv.inv0;
        calculateGrowth(&inv);
        sendOutput(inv.invarray,
                   inv.years);
    }
    return 0;
}

void calculateGrowth(Investment *invp)
{
    int i;

    for (i = 1; i <= invp->years; i = i + 1)
        invp->invarray[i] = invp->growth * invp->invarray[i - 1];
}

int getUserInput(Investment *invp)
{
    int valid;
    char output[BUFF_LEN];
    char input[BUFF_LEN];

    memset(output, '\0', BUFF_LEN * sizeof(char));
    sprintf(output, "Enter investment, growth rate, number of yrs (up to %d): ", MAX_YEARS);
    NU32DIP_WriteUART1(output);

    memset(input, '\0', BUFF_LEN * sizeof(char));
    NU32DIP_ReadUART1(input, BUFF_LEN);
    sscanf(input, "%lf %lf %d", &(invp->inv0), &(invp->growth), &(invp->years));

    valid = (invp->inv0 > 0) && (invp->growth > 0) &&
            (invp->years > 0) && (invp->years <= MAX_YEARS);

    memset(output, '\0', BUFF_LEN * sizeof(char));
    sprintf(output, "Valid input?  %d\r\n", valid);
    NU32DIP_WriteUART1(output);

    if (!valid)
    {
        memset(output, '\0', sizeof(char));
        sprintf(output, "Invalid input; exiting.\r\n");
        NU32DIP_WriteUART1(output);
    }
    return (valid);
}

void sendOutput(double *arr, int yrs)
{
    int i;
    char outstring[100];
    char output[BUFF_LEN];

    memset(output, '\0', BUFF_LEN * sizeof(char));
    sprintf(output, "\nRESULTS:\r\n\n");
    NU32DIP_WriteUART1(output);

    for (i = 0; i <= yrs; i++)
    {
        memset(outstring, '\0', 100 * sizeof(char));
        sprintf(outstring, "Year %3d:  %10.2f\r\n", i, arr[i]);
        NU32DIP_WriteUART1(outstring);
    }

    memset(output, '\0', BUFF_LEN * sizeof(char));
    sprintf(output, "\n");
    NU32DIP_WriteUART1(output);
}
