#ifndef HAND_h
#define HAND_h

#include "stdbool.h"

#include "utils/dynamic_string.h"
#include "utils/indexed_char_pointer.h"
#include "utils/dynamic_number_stack.h"
#include "utils/symbols.h"

// * //

bool is_syntax_panic_exit = false;
#define syntax_panic(printf_args...) do {is_syntax_panic_exit = true; panic(printf_args);} while(false);

typedef struct ChainHandlersState
{
    Str *exp;

    bool point_flag;
    size_t bracket_number;

    Str correct_symbols;
    Str correct_syntax;
} HanState;

typedef void (*chain_handler)(Chp* c, HanState *state);

HanState *new__hanstate(Str expression, Str correct_symbols, Str correct_syntax)
{
    HanState *self = malloc(sizeof(HanState));
    *self = (HanState){
        .exp = clone__str(expression),

        //

        .point_flag = false,
        .bracket_number = 0,

        .correct_symbols = correct_symbols,
        .correct_syntax = correct_syntax};

    return self;
}

void free__hanstate(HanState *self)
{
    free__str(self->exp);
    free(self);
}

// * //

Str *get_result__hanstate(HanState *self)
{
    return clone__str(*self->exp);
}

// * //

void clear_space_symbols(Chp* c, HanState *state)
{
    while (!is_none__chp(*c) && strchr(spaces__char, get__chp(*c)))
        remove_char__str(state->exp, c->index);
}

void check_correct_symbols(Chp* c, HanState *state)
{
    if (find_char__str(state->correct_symbols, 0, -1, get__chp(*c)) == -1)
            syntax_panic("wrong symbol : '%c' (index %d)", get__chp(*c), c->index);
}

void check_point(Chp* c, HanState *state)
{
    if (!is_digit(get__chp(prev__chp(*c))) && get__chp(prev__chp(*c)) != point__char)
        state->point_flag = false;
    
    if (get__chp(*c) == point__char)
    {
        if (state->point_flag)
            syntax_panic("double point (ind %d)", c->index);

        state->point_flag = true;
    }
}

void check_bracket(Chp* c, HanState *state)
{
    if (get__chp(*c) == bracket_start__char)
        state->bracket_number += 1;

    if (get__chp(*c) == bracket_end__char)
    {
        if (state->bracket_number == 0)
            syntax_panic("unpair bracket (ind %d)", c->index);
        state->bracket_number -= 1;
    }

    if (is_none__chp(next__chp(*c)) && state->bracket_number != 0)
        syntax_panic("unpair bracket (ind %d)", c->index);
}

// "abc efg"
bool _is_pattern(Chp p, char* pattern, bool left_contains, bool right_contains)
{
    char l = merge_digit(get__chp(prev__chp(p)));
    char r = merge_digit(get__chp(p));

    char pat[strlen(pattern)];
    strcpy(pat, pattern);

    char* sep = strchr(pat, ' ');
    *sep = '\0';

    char* lp = strchr(pat, l);
    char* rp = strchr(sep+1, r);
    *sep = ' ';

    bool lb = lp != NULL;
    bool rb = rp != NULL;

    return lb == left_contains && rb == right_contains;
}

void wrap_numbers(Chp* c, HanState *state)
{
    if (c->index == 0 || is_none__chp(prev__chp(*c)))
        return;

    size_t old_len = len__str(*state->exp);

    if (_is_pattern(*c, "d +-", true, true))
        insert__str(state->exp, init__str(";+:"), c->index);

    else if (_is_pattern(*c, "d d.", true, false))
        insert__str(state->exp, init__str(";"), c->index);
    
    else if (_is_pattern(*c, "(: +-", false, true))
        insert__str(state->exp, init__str("+:"), c->index);

    else if (_is_pattern(*c, "+- (", true, true))
        insert__str(state->exp, init__str("1;*"), c->index);

    else if (_is_pattern(*c, "(* +-", true, true))
        insert__str(state->exp, init__str(":"), c->index);

    else if (_is_pattern(*c, "+-d. d", false, true))
        insert__str(state->exp, init__str(":+"), c->index);
    
    c->index += len__str(*state->exp) - old_len;
}

void check_correct_syntax(Chp* c, HanState *state)
{
    if (c->index == 0 || is_none__chp(prev__chp(*c)))
        return;

    Str it = init__str((char[]){merge_digit(get__chp(prev__chp(*c))), merge_digit(get__chp(*c)), '\0'});
    if (find__str(state->correct_syntax, 0, -1, it) == -1)
        syntax_panic("wrong syntax \"%s\" (ind %d)", it.buffer, c->index);
}

// * //

void _print_intermediate_exp(Chp *c, HanState *state)
{
    printf("handle \t\t: %3d '%c' : '%s'\n", c->index, get__chp(*c), state->exp->buffer);
}

Str *prehandle_expression(Str expression)
{
    chain_handler handlers[] = {
        clear_space_symbols,
        check_correct_symbols,
        check_correct_syntax,
        check_point,
        check_bracket,
        wrap_numbers

#ifdef LOG_PRINT_ON
        /*/*/,
        _print_intermediate_exp
#endif

    };
    size_t handlers_len = sizeof(handlers) / sizeof(chain_handler);

    Str *correct_symbols = new__str("+-* () \t\n .1234567890");
    Str *correct_syntax = new__str(" \
        dd d. .d \
        (d d) \
        *d +d -d d- d+ d* \
        *( +( -( )- )+ )*\
        (( )) (+ (- \
    ");

    HanState *state = new__hanstate(expression, *correct_symbols, *correct_syntax);

#ifdef LOG_PRINT_ON
    remove_char__str(state->exp, -1);
#endif

    insert_char__str(state->exp, bracket_start__char, 0);
    push_char__str(state->exp, bracket_end__char);

    for (Chp c = init__chp(state->exp, 0); !is_none__chp(c); c = offset__chp(c, 1))
        for (size_t i = 0; i < handlers_len; ++i)
            handlers[i](&c, state);

    Str *res = get_result__hanstate(state);

#ifdef LOG_PRINT_ON
    printf("handle !end! \t\t: '%s'\n", res->buffer);
#endif

    free__str(correct_symbols);
    free__str(correct_syntax);
    free__hanstate(state);

    return res;
}

// * //
#endif