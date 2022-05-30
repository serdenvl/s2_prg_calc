#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

// #define DEBUG_ON       //
#define LOG_PRINT_ON    //
#define MEMORY_CHECK_ON //

// #define strange_crutch //
#ifdef strange_crutch

void free__crutch(void *pointer)
{
}
#define free(pointer) free__crutch(pointer)

void *realloc__crutch(void *pointer, size_t size)
{
    return memcpy(malloc(size), pointer, size);
}
#define realloc(pointer, size) realloc__crutch(pointer, size)

#endif

#ifdef MEMORY_CHECK_ON
#ifndef DEBUG_ON
#include "utils/memory_check.h"
#endif
#endif

#include "prehandlers.h"
#include "postfix.h"
#include "calculate.h"
#include "utils/symbols.h"
#include "utils/number.h"
#include "utils/dynamic_string.h"

// * //

void do_task()
{
    char buf;
    Str *sbuf = new__str("");

    Str *prehandled;
    Str *postfix;
    Num *number;
    Str *answer;

    while (true)
    {
        red_print("Введите выражение: ");

        do
        {
            buf = getchar();
            push_char__str(sbuf, buf);
        } while (buf != '\n');

        if (contain__str(*sbuf, init__str("end")))
            break;

        prehandled = prehandle_expression(*sbuf);
        postfix = to_postfix(*prehandled);
        number = calculate(*postfix);
        answer = to_str__num(*number);

        printf("Ответ: %s\n\n", array__str(*answer));

        remove__str(sbuf, 0, -1);
        free__str(prehandled);
        free__str(postfix);
        free__num(number);
        free__str(answer);

#ifdef DEBUG_ON
        break;
#endif
    }

    free__str(sbuf);
}

// * //

#define task_name "lab calc"
#define welcome_message "\
Калькулятор чисел с плавающей точкой неограниченного размера \n\
Поддерживает операции +, -, * и взятие в скобки \n\
"

//
void prevent_syntax_panic_exit()
{
#ifdef DEBUG_ON
    return;
#endif

    if (!is_syntax_panic_exit)
        return;
    is_syntax_panic_exit = false;
    atexit(prevent_syntax_panic_exit);

    printf("");

    do_task();
}

int main()
{
    localize();
    welcome(task_name, welcome_message);
    red_print("( для выхода из цикла введите 'end' )");

#ifdef MEMORY_CHECK_ON
    start__memcheck();
#endif

    atexit(prevent_syntax_panic_exit);

    do_task();

    // getchar();
}
