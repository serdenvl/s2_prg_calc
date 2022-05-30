#ifndef STR_h
#define STR_h

#include <malloc.h>
#include <string.h>
#include <stdbool.h>

#include "stuffs.h"

// * //

typedef struct DynamicCharArray
{
    char *buffer;
    size_t size;
    size_t capacity;

    bool is_static;
} Str;

#define expance_step 8
#define expance_cap(new_size) expance_step *(new_size / expance_step) + expance_step *(new_size % expance_step != 0)

Str init__str(char *str)
{
    size_t size = strlen(str) + 1;
    return (Str){
        .buffer = str,
        .size = size,
        .capacity = size,
        .is_static = true};
}

#define init_char__str(ch) init__str((char[]){ch, '\0'})
int i = 0;
Str *clone__str(Str self)
{
    i++;
    if(i == 7)
        i = i;
    Str *new_str = malloc(sizeof(Str));
    *new_str = self;

    new_str->buffer = malloc(sizeof(char) * self.size);
    strcpy(new_str->buffer, self.buffer);

    new_str->is_static = false;

    return new_str;
}

Str *new__str(char *str)
{
    return clone__str(init__str(str));
}

void free__str(Str *self)
{
    if(!self)
        return;
        
    if (self->is_static)
        panic("free static memory");
    free(self->buffer);
    free(self);
}

int check0 = 0;
Str *_resize__str(Str *self, size_t new_size)
{
    check0 += 1;

    if (self->capacity < new_size)
    {
        if (self->is_static)
            panic("change static memory (self: %x, buff: %x)", self, self->buffer);

        self->capacity = expance_cap(new_size);
        self->buffer = (char *)realloc(self->buffer, self->capacity*(sizeof(char)));
    }

    self->size = new_size;
    self->buffer[self->size - 1] = '\0';

    return self;
}

// * // indexing

int format_index__str(Str self, int index)
{
    if (index < 0)
        index += self.size - 1;
    if (!(0 <= index && index < self.size-1))
        return -1;
    return index;
}

#define _handle_index(self, index, panics...)           \
    do                                                  \
    {                                                   \
        int index##_t = format_index__str(self, index); \
        if (index##_t == -1)                            \
            panic(panics);                              \
        index = index##_t;                              \
    } while (false);

// * // getters

char *array__str(Str self)
{
    return self.buffer;
}

char get_char__str(Str self, int index)
{
    _handle_index(self, index, "index out of range (%d of [0, %d]) \"%s\"", index, self.size - 1, self.buffer);
    return self.buffer[index];
}

size_t len__str(Str self)
{
    return self.size - 1;
}

size_t size__str(Str self)
{
    return self.size;
}

bool is_empty__str(Str *s)
{
    return s->size < 2;
}

int find__str(Str self, int start, int end, Str substr)
{
    _handle_index(self, start, "index out of range (%d of [0, %d]) \"%s\"", start, self.size - 1, self.buffer);
_handle_index(self, end, "index out of range (%d of [0, %d]) \"%s\"", end, self.size - 1, self.buffer);

    char *str = array__str(self);
    char *sub = array__str(substr);
    size_t l = len__str(substr);

    char* ind;

    for(ind = strchr(str+start, *sub); ind && ind+l-1 <= str+end; ind = strchr(ind + 1, *sub))
    {
        if (strncmp(ind, sub, l) == 0)
            return ind - str;
    }

    return -1;
}

int find_char__str(Str self, int start, int end, char ch)
{
    return find__str(self, start, end, init_char__str(ch));
}

bool contain__str(Str self, Str substr)
{
    return find__str(self, 0, -1, substr) != -1;
}

bool contain_char__str(Str self, char ch)
{
    return find_char__str(self, 0, -1, ch) != -1;
}

// * // add

Str *insert__str(Str *self, Str str, int index)
{
    if (index != len__str(*self))
        _handle_index(*self, index, "index is out of range (ind %d, len %d)", index, len__str(*self));

    _resize__str(self, self->size + str.size - 1);

    char buffer[self->size - index];
    strcpy(buffer, self->buffer + index);
    self->buffer[index] = '\0';

    strcat(self->buffer, str.buffer);
    strcat(self->buffer, buffer);

    return self;
}

Str *insert_char__str(Str *self, char ch, int index)
{
    return insert__str(self, init_char__str(ch), index);
}

Str *push__str(Str *self, Str str)
{
    return insert__str(self, str, len__str(*self));
}

Str *push_char__str(Str *self, char ch)
{
    return push__str(self, init_char__str(ch));
}

// * // rem

Str *remove__str(Str *self, int index_start, int index_end)
{
    _handle_index(*self, index_start, "index is out of range (ind %d, len %d)", index_start, len__str(*self));
    _handle_index(*self, index_end, "index is out of range (ind %d, len %d)", index_end, len__str(*self));

    if (index_end < index_start)
        panic("uncorrect range [%d;%d]", index_start, index_end);

    self->size -= index_end - index_start + 1;
    self->buffer[index_start] = '\0';
    strcpy(self->buffer + index_start, self->buffer + index_end + 1);
    return self;
}

char remove_char__str(Str *self, size_t index)
{
    char buff = get_char__str(*self, index);
    remove__str(self, index, index);
    return buff;
}

char pop_char__str(Str *self)
{
    return remove_char__str(self, len__str(*self) - 1);
}

// * //

Str *substr__str(Str self, int start_index, int end_index)
{
    _handle_index(self, start_index, "index is out of range (ind %d, len %d)", start_index, len__str(self));
    _handle_index(self, end_index, "index is out of range (ind %d, len %d)", end_index, len__str(self));

    if (!(start_index <= end_index))
        panic("wrong range");

    size_t len = end_index - start_index + 1;
    char buff[len + 1];
    strncpy(buff, array__str(self) + start_index, len);
    buff[len] = '\0';
    return new__str(buff);
}

Str* cut_substr__str(Str* self, int start_index, int end_index)
{
    Str* res = substr__str(*self, start_index, end_index);
    remove__str(self, start_index, end_index);
    return res;
}

Str *reverse__str(Str *self)
{
    char *first = self->buffer;
    char *last = self->buffer + len__str(*self) - 1;
    char buf;
    for (size_t i = 0; i < len__str(*self); ++i)
    {
        buf = *first;
        *first = *last;
        *last = buf;
    }
    return self;
}

// * //
#endif