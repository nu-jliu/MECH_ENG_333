/**
 * @file bubble.c
 * @author Allen Liu (jingkunliu2025@u.northwestern.edu)
 * @brief Takes a string as input and sort the string based on ascii in ascending order.
 * @version 0.1
 * @date 2024-01-07
 *
 * @copyright Copyright (c) 2024
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define BUFF_SIZE 256

int main(int, char **);
int getString(char *);
void printResult(char *);
bool greaterThan(char, char);
void swap(char *, char *);

/**
 * @brief The main function of the program, that sort the string.
 *
 * @param argc The number of arguments.
 * @param argv The value of arguments.
 * @return int the program result code.
 */
int main(int argc, char **argv)
{
    char *buff = malloc(BUFF_SIZE * sizeof(char));
    memset(buff, '\0', BUFF_SIZE * sizeof(char));

    if (getString(buff) == EOF)
    {
        perror("Error reading the input string");
        return EXIT_FAILURE;
    }

    int len_str = strlen(buff);

    for (int i = 0; i < len_str; i++)
    {
        for (int j = i; j < len_str; j++)
        {
            char first = buff[i];
            char second = buff[j];

            if (greaterThan(first, second))
                swap(buff + i * sizeof(char), buff + j * sizeof(char));
        }
    }

    printResult(buff);
    free(buff);

    return 0;
}

/**
 * @brief Get the string to sort.
 *
 * @param buff Address to the buffer to store the string.
 * @return int number of items assigned, or EOF when not
 */
int getString(char *buff)
{
    printf("Enter the string you would like to sort: ");
    return scanf("%s", buff);
}

/**
 * @brief Display the result of the sorted string.
 *
 * @param buff The adress where stores the result string.
 */
void printResult(char *buff)
{
    printf("Here is the sorted string: %s\n", buff);
}

/**
 * @brief Compare two characters
 *
 * @param first The first character
 * @param second The second character
 * @return true if the first is greater than second
 * @return false if the first is not greater than second
 */
bool greaterThan(char first, char second)
{
    return first > second;
}

/**
 * @brief Swap the value between two location.
 *
 * @param first The address of the first location
 * @param second The adress of the second one.
 */
void swap(char *first, char *second)
{
    char tmp = *first;
    *first = *second;
    *second = tmp;
}