#ifndef NUM_h
#define NUM_h

#include "dynamic_string.h"
#include "indexed_char_pointer.h"
#include "symbols.h"

// * //

typedef struct Number
{
    Str *numstr;
    size_t point_index;

    bool sign;
    bool is_invert;
} Num;

Num *clone__num(Num self)
{
    Num *n = malloc(sizeof(Num));
    *n = self;
    n->numstr = clone__str(*self.numstr);

    return n;
}

Num _init__num(Str *number, bool sign)
{
    int point_index = find_char__str(*number, 0, -1, point__char);
    if (point_index == -1)
        panic("no point");

    return (Num){
        .numstr = number,
        .point_index = point_index,

        .sign = sign,
        .is_invert = false};
}

#define init__num(number, sign) _init__num((Str *)((Str[]){number}), sign)

Num *new__num(Str number, bool sign)
{
    return clone__num(_init__num(&number, sign));
}

void free__num(Num *self)
{
    free__str(self->numstr);
    free(self);
}

Num *from_format_str__num(Str number)
{
    if (get_char__str(number, 0) != num_start__char)
        panic("wrong num start");
    if (get_char__str(number, -1) != num_end__char)
        panic("wrong num end");
    if (get_char__str(number, 1) != plus__char && get_char__str(number, 1) != minus__char)
        panic("wrong sign");

    bool sign = get_char__str(number, 1) == plus__char;

    int point_index = find_char__str(number, 0, -1, point__char);

    Num *self = malloc(sizeof(Num));
    *self = (Num){
        .numstr = substr__str(number, 2, -2),
        .point_index = point_index - 2,

        .sign = sign,
        .is_invert = false};

    if (point_index == -1)
    {
        push_char__str(self->numstr, point__char);
        self->point_index = format_index__str(*self->numstr, -1);
    }

    return self;
}

Num *_exchange__num(Num *self, int max_ind);
Num *strip__num(Num *self);

// * // getters

size_t len_tail__num(Num self)
{
    return len__str(*self.numstr) - 1 - self.point_index;
}

size_t len_body__num(Num self)
{
    return self.point_index;
}

Str* to_str__num(Num self)
{
    Num* n = clone__num(self);
    strip__num(n);

    Str* res = new__str("");

    push_char__str(res, n->sign ? plus__char : minus__char);

    if(len_body__num(*n) == 0)
        push_char__str(res, '0');

    push__str(res, *n->numstr);
    
    if(len_tail__num(*n) == 0)
        pop_char__str(res);

    free__num(n);
    return res;
}

Str* to_format_str__num(Num self)
{
    Str* res = to_str__num(self);
    insert_char__str(res, num_start__char, 0); 
    push_char__str(res, num_end__char);
    return res;
}

int format_index__num(Num self, int index)
{
    if (index == 0 || (index > 0 && index > len_body__num(self)) || (index < 0 && -index > len_tail__num(self)))
        return -1;
    return self.point_index - index;
}

Num *set_digit__num(Num *self, int index, int digit);
Num *new_int__num(int n, bool sign)
{
    Num *res = new__num(init_char__str(point__char), sign);

    if (n < 0)
        n = -n;

    for (; n > 0; n /= 10)
        set_digit__num(res, len_body__num(*res) + 1, n % 10);

    return res;
}

#define init_int__num(n, sign) init__num(init__str(#n "."), sign)

int get_digit__num(Num self, int index)
{
    if (index == 0)
        panic("index [+1..body_len] [-1..tail_len]");

    int ind = format_index__num(self, index);
    if (ind != -1)
        ind = format_index__str(*self.numstr, ind);

    int digit = (ind != -1) ? (get_char__str(*self.numstr, ind) - '0') : 0;

    return !self.is_invert ? digit : 9 - digit;
}

// * // setters

Num *set_digit__num(Num *self, int index, int digit)
{
    if (index == 0)
        panic("0 index");
    _exchange__num(self, index);

    array__str(*self->numstr)[format_index__num(*self, index)] = '0' + (!self->is_invert ? digit : 9 - digit);
    return self;
}

Num *set_point__num(Num *self, int index)
{
    if (index == 0)
        return self;
    _exchange__num(self, index);

    remove_char__str(self->numstr, self->point_index);
    
    int ind = format_index__num(*self, index);
    self->point_index -= index;

    if(ind != -1)
        insert_char__str(self->numstr, point__char, ind);
    else if(index > 0)
        insert_char__str(self->numstr, point__char, 0);
        else if(index < 0)
        push_char__str(self->numstr, point__char);

    return self;
}

// * // stranges

Num *_exchange__num(Num *self, int max_ind)
{
    if (max_ind > 0 && len_body__num(*self) < max_ind)
    {
        for (size_t i = len_body__num(*self); i < max_ind; ++i)
            insert_char__str(self->numstr, '0', 0);
        self->point_index += max_ind - len_body__num(*self);
    }
    if (max_ind < 0 && len_tail__num(*self) < -max_ind)
        for (size_t i = len_tail__num(*self); i < -max_ind; ++i)
            push_char__str(self->numstr, '0');
    return self;
}

Num *strip__num(Num *self)
{
    size_t count = 0;

    for (size_t i = 0; i < len__str(*self->numstr) && get_char__str(*self->numstr, i) == '0'; ++i)
        ++count;

    if (count != 0)
    {
        remove__str(self->numstr, 0, count - 1);
        self->point_index -= count;
    }

    count = 0;
    for (size_t i = len__str(*self->numstr) - 1; 0 <= i && get_char__str(*self->numstr, i) == '0'; --i)
        ++count;

    if (count != 0)
        remove__str(self->numstr, len__str(*self->numstr) - count, len__str(*self->numstr) - 1);

    return self;
}

Num *exfree__num(Num *old, Num *new)
{
    free__num(old);
    return new;
}

// * // arithmetic

Num *summ__num(Num a, Num b)
{
    int start_index = -max(len_tail__num(a), len_tail__num(b));
    int end_index = max(len_body__num(a), len_body__num(b));
    bool subt_flag = a.sign != b.sign;

    Num *res = new_int__num(0, a.sign);

    if (subt_flag)
    {
        a.is_invert = true;
        res->is_invert = true;
    }

    int digsum = 0;
    int trans = 0;
    for (int i = start_index; i <= end_index; ++i)
    {
        if (i == 0)
            continue;
        digsum = get_digit__num(a, i) + get_digit__num(b, i) + trans;
        trans = digsum / 10;
        set_digit__num(res, i, digsum % 10);
    }

    if (trans != 0)
    {
        if (subt_flag)
        {
            res->sign = !res->sign;
            Num* stangeone = new_int__num(1, res->sign);
            set_point__num(stangeone, len_tail__num(*res)); // ??? -len
            return exfree__num(stangeone, exfree__num(res, summ__num(*stangeone, *res)));
        }
        set_digit__num(res, len_body__num(*res) + 1, 1);
    }

    res->is_invert = false;
    strip__num(res);
    return res;
}

Num *prod__num(Num a_, Num b_)
{
    Num *a = strip__num(clone__num(a_));
    Num *b = strip__num(clone__num(b_));

    if ((len_body__num(*a) == 0 && len_tail__num(*a) == 0) || (len_body__num(*b) == 0 && len_tail__num(*b) == 0))
    {
        free__num(a);
        free__num(b);
        return new_int__num(0, true);
    }

    set_point__num(a, len_tail__num(*b));
    set_point__num(b, -len_tail__num(*b));

    Num one = init_int__num(1, !b->sign);
    Num* sum = clone__num(*a);

    while (!(len_body__num(*b) == 1 && get_digit__num(*b, 1) == 1))
    {
        sum = exfree__num(sum, summ__num(*sum, *a));
        b = exfree__num(b, summ__num(*b, one));
    }

    sum->sign = a->sign == b->sign;
    strip__num(sum);

    free__num(a);
    free__num(b);
    return sum;
}

// * //
#endif