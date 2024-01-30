#ifndef IO_H
#define IO_H

#define MAX_YEARS 100

typedef struct
{
    double inv0;
    double growth;
    int years;
    double invarray[MAX_YEARS + 1];
} Investment;

int getUserInput(Investment *);
void sendOutput(double *, int);

#endif