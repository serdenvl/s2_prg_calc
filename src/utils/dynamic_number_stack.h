#ifndef NUMSTACK_h
#define NUMSTACK_h

#include <malloc.h>
#include <stdbool.h>

#include "stuffs.h"
#include "number.h"

// * //

typedef struct _DynamicNumberStack_element
{
    Num* val;

    void* down; // NumStackEl*
} NumStackEl;

NumStackEl* new__numstackel(Num* val, NumStackEl* down)
{
    NumStackEl* self = malloc(sizeof(NumStackEl));
    *self = (NumStackEl){
        .val = val,

        .down = down
    };
    return self;
}

Num* get_value__numstackel(NumStackEl self)
{
    return clone__num(*self.val);
}

void free__numstackel(NumStackEl* self, bool free_tail)
{
    if(!self)
        return;
    free__num(self->val);

    if(free_tail && self->down)
        free__numstackel(self->down, true);
    free(self);
}

// * //

typedef struct DynamicNumberStack
{
    NumStackEl* top;
} NumStack;

NumStack* new__numstack()
{
    NumStack* self = malloc(sizeof(NumStack));
    *self = (NumStack){
        .top = NULL
    };
    return self;
}

void free__numstack(NumStack* self)
{
    if(!self)
        return;
    free__numstackel(self->top, true);
    free(self);
}

// * // get

Num* top__numstack(NumStack* self)
{
    return self->top->val;
}

// * // add

void push__numstack(NumStack* self, Num* it)
{
    self->top = new__numstackel(it, self->top);
}

// * // rem

Num* pop__numstack(NumStack* self)
{
    NumStackEl* top = self->top;

    Num* res = get_value__numstackel(*top);
    self->top = top->down;

    free__numstackel(top, false);

    return res;
}

// * //
#endif