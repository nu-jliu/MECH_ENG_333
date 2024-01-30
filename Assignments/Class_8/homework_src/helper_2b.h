#ifndef HELPER_H
#define HELPER_H

#define MAX_YEARS 100

typedef struct {
    double inv0;
    double growth;
    int years;
    double invarray[MAX_YEARS + 1];
} Investment;

int getUserInput(Investment *);
void calculateGrowth(Investment *);
void sendOutput(double *, int);

#endif