#ifndef SYMS_h
#define SYMS_h

#include "stuffs.h"

// * //

#define num_start__char ':'
#define num_end__char ';'

#define bracket_start__char '('
#define bracket_end__char ')'

#define plus__char '+'
#define minus__char '-'
#define times__char '*'

#define point__char '.'
#define digit__char 'd'

#define space__char ' '
#define spaces__char " \t\n"

char merge_digit(char c)
{
    return (is_digit(c)) ? digit__char : c;
}

// * //
#endif