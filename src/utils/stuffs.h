#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <malloc.h>
#include <windows.h>

// * //

// * //     output

int red_print(char *message)
{
    if (message[0] != '\n')
        printf("\n");
    printf(message);
}

void welcome(char *task_name, char *description)
{
    red_print(task_name);
    printf("\nЗадание: ");
    red_print(description);
    printf("\n");
}

void localize()
{
    SetConsoleCP(65001);
    SetConsoleOutputCP(65001);
}

#define panic(printf_args...)      \
    do                             \
    {                              \
        printf("\nerror : ");      \
        printf(__FUNCTION__);      \
        printf(" : " printf_args); \
        printf("\n");              \
        exit(0);                   \
    } while (0);

// * //     scan

int scan_int_limit(char *message, int min, int max)
{
    int buff;
    do
    {
        if (message != "")
            red_print(message);
        scanf("%d", &buff);

        if (buff < min)
            printf("\nЗначение должно быть не меньше %d", min);
        if (buff > max)
            printf("\nЗначение должно быть не больше %d", max);
    } while (buff < min || max < buff);

    return buff;
}

int scan_int(char *message)
{
    return scan_int_limit(message, -__INT_MAX__, __INT_MAX__);
}

// * //     input

int question(char *message)
{
    while (1)
    {
        red_print(message);
        printf(" (Y/N):");

        switch (getchar())
        {
        case 'n':
        case 'N':
            return 0;
        case 'y':
        case 'Y':
            return 1;
        default:
            while (getchar() != '\n')
                ;
            printf("Wrong answer");
        }
    }
}

int scan_variant(char *message, char *variants[], int length)
{
    int buff;
    while (1)
    {
        if (message != "")
            red_print(message);
        for (int i = 0; i < length; ++i)
            printf("\n%d - %s", i, variants[i]);

        printf("\n");
        buff = scan_int("");
        if (0 <= buff && buff < length)
            return buff;

        printf("Неверный ответ\n");
        while (getchar() != '\n')
            ;
    }
}

// * //

bool is_digit(char ch) { return '0' <= ch && ch <= '9'; }

typedef void (*free_fun)(void *);

// * //
#endif