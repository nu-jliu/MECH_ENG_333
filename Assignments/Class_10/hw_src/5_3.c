#include <xc.h>
#include "nu32dip.h"

int main(void)
{
    // NU32DIP_Startup();

    char c1 = 5, c2 = 6, c3;
    c3 = c1 + c2;
    c3 = c1 - c2;
    c3 = c1 * c2;
    c3 = c1 / c2;

    int i1 = 5, i2 = 6, i3;
    i3 = i1 + i2;
    i3 = i1 - i2;
    i3 = i1 * i2;
    i3 = i1 / i2;

    long long int j1 = 5, j2 = 6, j3;
    j3 = j1 + j2;
    j3 = j1 - j2;
    j3 = j1 * j2;
    j3 = j1 / j2;

    float f1 = 1.01, f2 = 2.02, f3;
    f3 = f1 + f2;
    f3 = f1 - f2;
    f3 = f1 * f2;
    f3 = f1 / f2;

    long double d1 = 1.01, d2 = 2.02, d3;
    d3 = d1 + d2;
    d3 = d1 - d2;
    d3 = d1 * d2;
    d3 = d1 / d2;

    return 0;
}