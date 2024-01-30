#include "helper.h"

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