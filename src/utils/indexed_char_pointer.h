#ifndef CHP_H
#define CHP_H

#include "dynamic_string.h"

// * //

typedef struct IndexedCharPointer 
{
    Str* source;
    int index;
} Chp;

Chp init__chp(Str* source, int index)
{
    return (Chp){
        .source = source,
        .index = index
    };
}

// * // get

char get__chp(Chp self)
{
    size_t index = format_index__str(*self.source, self.index);
    if(index == -1)
        panic("get none")
    return get_char__str(*self.source, index);
}

bool is_none__chp(Chp self)
{
    int ind = format_index__str(*self.source, self.index);
    if(ind != -1 && self.source->buffer[ind] == '\0')
    {
        ind = -ind;
        ind = -ind;
    }
    return format_index__str(*self.source, self.index) == -1;
}

bool in_chars__chp(Chp self, char* chars)
{
    return strchr(chars, get__chp(self)) != NULL;
}

// * // ind

Chp offset__chp(Chp p, size_t offset)
{
    return init__chp(p.source, p.index+offset);
}

Chp prev__chp(Chp p)
{
    p.index -= 1;
    return p;
}

Chp next__chp(Chp p)
{
    return offset__chp(p, 1);
}

// * //
#endif