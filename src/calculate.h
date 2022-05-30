#include "utils/symbols.h"

// * //

Num *calculate(Str postfix_exp)
{
    Str *exp = clone__str(postfix_exp);
    NumStack *stack = new__numstack();

    Str *sbuf;
    Num *a;
    Num *b;

    for (Chp c = init__chp(exp, 0); !is_none__chp(c); c = offset__chp(c, 1))
    {

#ifdef LOG_PRINT_ON
        printf("calculate \t: %3d '%c' : '%s' : stack", c.index, get__chp(c), exp->buffer);
        printf(" : (");
        if (stack->top)
            printf("%c%s",
                   stack->top->val->sign ? '+' : '-',
                   stack->top->val->numstr->buffer);
        if (stack->top && stack->top->down)
            printf(", %c%s",
                   ((NumStackEl *)stack->top->down)->val->sign ? '+' : '-',
                   ((NumStackEl *)stack->top->down)->val->numstr->buffer);
        printf(")");
#endif

        switch (get__chp(c))
        {
        case num_start__char:
            sbuf = substr__str(*exp, c.index, find_char__str(*exp, c.index, -1, num_end__char));
            push__numstack(stack, from_format_str__num(*sbuf));
            c = offset__chp(c, len__str(*sbuf) - 1);

            free__str(sbuf);
            break;

        case plus__char:
            b = pop__numstack(stack);
            a = pop__numstack(stack);
            push__numstack(stack, summ__num(*a, *b));
            free__num(a);
            free__num(b);
            break;

        case times__char:
            b = pop__numstack(stack);
            a = pop__numstack(stack);
            push__numstack(stack, prod__num(*a, *b));
            free__num(a);
            free__num(b);
            break;

        default:
            panic("impossible error : uncaught symbol '%c'", get__chp(c));
        }

#ifdef LOG_PRINT_ON
        printf(" -> (");
        if (stack->top)
            printf("%c%s",
                   stack->top->val->sign ? '+' : '-',
                   stack->top->val->numstr->buffer);
        if (stack->top && stack->top->down)
            printf(", %c%s",
                   ((NumStackEl *)stack->top->down)->val->sign ? '+' : '-',
                   ((NumStackEl *)stack->top->down)->val->numstr->buffer);
        printf(")\n");
#endif
    }

    Num *res = pop__numstack(stack);

#ifdef LOG_PRINT_ON
    printf("calculate !end! \t\t: '%s'\n", res->numstr->buffer);
#endif

    free__str(exp);
    free__numstack(stack);

    return res;
}
