#include "utils/symbols.h"

void _pushpop_until_contains_or_empty(char *pattern, Str *spush, Str *spop)
{
    while (!is_empty__str(spop) && !strchr(pattern, get_char__str(*spop, -1)))
        push_char__str(spush, pop_char__str(spop));
}

Str *to_postfix(Str expression)
{
    Str *exp = clone__str(expression);
    Str *stack = new__str("");
    Str *res = new__str("");

    Str *sbuf;

    for (Chp c = init__chp(exp, 0); !is_none__chp(c); c = offset__chp(c, 1))
    {

#ifdef LOG_PRINT_ON
        printf("postfix \t: %3d '%c' : stack & exp", c.index, get__chp(c));
        printf(" : \t'%s' & '%s'", array__str(*stack), array__str(*exp));
#endif

        switch (get__chp(c))
        {
        case num_start__char:
            sbuf = substr__str(*exp, c.index, find_char__str(*exp, c.index, -1, num_end__char));

            push__str(res, *sbuf);
            c = offset__chp(c, len__str(*sbuf) - 1);

            free__str(sbuf);
            break;

        case bracket_start__char:
            push_char__str(stack, bracket_start__char);
            break;
        case bracket_end__char:
            _pushpop_until_contains_or_empty("(", res, stack);
            pop_char__str(stack);
            break;

        case plus__char:
            _pushpop_until_contains_or_empty("(", res, stack);
            push_char__str(stack, plus__char);
            break;

        case times__char:
            _pushpop_until_contains_or_empty("(+", res, stack);
            push_char__str(stack, times__char);
            break;

        default:
            panic("impossible error : uncaught symbol '%c'", get__chp(c));
        }

#ifdef LOG_PRINT_ON
        printf(" -> '%s' & '%s'\n", array__str(*stack), array__str(*exp));
#endif
    }
    _pushpop_until_contains_or_empty("", res, stack);

#ifdef LOG_PRINT_ON
    printf("postfix !end! \t\t: '%s'\n", res->buffer);
#endif

    free__str(exp);
    free__str(stack);
    return res;
}