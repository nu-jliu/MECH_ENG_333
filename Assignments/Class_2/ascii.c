#include <stdio.h>
#define ROW_SIZE 4

int main(int argc, char **argv)
{
    int counter = 0;

    for (int i = 33; i < 128; i++)
    {
        printf("%d: %c\t", i, (char)i);

        if (++counter == ROW_SIZE)
        {
            printf("\n");
            counter = 0;
        }
    }

    printf("\n");
}