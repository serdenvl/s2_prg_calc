#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

size_t __MEMORY_COUNT = 0;
bool __MEMCHECK_is_first_handle = true;

#ifndef MEMCHECK_LOGFILE_NAME
#define MEMCHECK_LOGFILE_NAME "../mem_check_log.txt"
#endif

#ifndef MEMSIZE_GET
#define MEMSIZE_GET(pointer) _msize(pointer)
#endif

void _atexit__memcheck()
{
    FILE *log = fopen(MEMCHECK_LOGFILE_NAME, "a");
    if (__MEMORY_COUNT != 0)
    {
        fprintf(log, "\nПамять не очищена (%d bytes)", __MEMORY_COUNT);
        printf("\nПамять не очищена (%d bytes)", __MEMORY_COUNT);
    }
    else
    {
        fprintf(log, "\nПамять успешно очищена.");
    }

    fclose(log);
}

#define _info_vars _info_file, _info_line, _info_funcname
#define _def_info_vars const char *_info_file, size_t _info_line, const char *_info_funcname
#define _get_info_vars __FILE__, __LINE__, __FUNCTION__

//
void *realloc_crutch(void *pointer, size_t size)
{
    void *res = memcpy(malloc(size), pointer, size);
    free(pointer);
    return res;
}
#define realloc(pointer, size) realloc_crutch(pointer, size)
//

// * //

void mem_change_handle__memcheck(char *type, int mem_diff, char *mem_info, _def_info_vars)
{
#define s1 "\n!%s!", type
#define s2 "\n\tinfo: %s at line %d (%s)", _info_vars
#define s3 "\n\tmem info: %s", mem_info
#define s4 "\n\tmem change: %+d  (%d -> %d)\n", mem_diff, __MEMORY_COUNT, __MEMORY_COUNT + mem_diff

    if (__MEMCHECK_is_first_handle)
    {
        __MEMCHECK_is_first_handle = false;

        fclose(fopen(MEMCHECK_LOGFILE_NAME, "w"));

        atexit(_atexit__memcheck);
    }

    FILE *log = fopen(MEMCHECK_LOGFILE_NAME, "a");
    fprintf(log, s1);
    fprintf(log, s2);
    fprintf(log, s3);
    fprintf(log, s4);
    fclose(log);

    __MEMORY_COUNT += mem_diff;
}

// * //

const size_t _addres_len = 11; // sizeof(void*)*2;

#define def_address_chars(name, pointer)              \
    char name[_addres_len + 3];                       \
    _sprintf_address(pointer, name + 2, _addres_len); \
    name[0] = '0';                                    \
    name[1] = 'x';                                    \
    name[_addres_len + 2] = '\0';

void _sprintf_address(void *pointer, char *buffer, size_t digit_count)
{
    char hexs[] = "0123456789ABCDEF";

    size_t p = (size_t)pointer;
    for (size_t i = 0; i < digit_count; ++i)
    {
        buffer[digit_count - 1 - i] = hexs[p & 15];
        p >>= 4;
    }
}

// * //

void _free_marked(void *pointer, _def_info_vars)
{
    def_address_chars(b, pointer);
    mem_change_handle__memcheck("free", -MEMSIZE_GET(pointer), b, _info_vars);

    free(pointer);
}

void *_malloc_marked(size_t size, _def_info_vars)
{
    void *res = malloc(size);

    def_address_chars(b, res);
    mem_change_handle__memcheck("malloc", MEMSIZE_GET(res), b, _info_vars);

    return res;
}

void *_calloc_marked(size_t number_of, size_t size, _def_info_vars)
{
    void *res = calloc(number_of, size);

    def_address_chars(b, res);
    mem_change_handle__memcheck("calloc", MEMSIZE_GET(res), b, _info_vars);

    return res;
}

void *_realloc_marked(void *pointer, size_t newsize, _def_info_vars)
{
    size_t oldsize = MEMSIZE_GET(pointer);
    def_address_chars(b1, pointer);

    void *res = realloc(pointer, newsize);

    def_address_chars(b2, res);

    char b3[strlen(b1) + 4 + strlen(b2) - 1];
    strcpy(b3, b1);
    strcpy(b3 + strlen(b1), " -> ");
    strcpy(b3 + strlen(b1) + 4, b2);

    mem_change_handle__memcheck("realloc", MEMSIZE_GET(res) - oldsize, b3, _info_vars);

    return res;
}

// * //

void _free_adapt(void *pointer, _def_info_vars) { return free(pointer); }
void *_malloc_adapt(size_t size, _def_info_vars) { return malloc(size); }
void *_calloc_adapt(size_t number_of, size_t size, _def_info_vars) { return calloc(number_of, size); }
void *_realloc_adapt(void *pointer, size_t newsize, _def_info_vars) { return realloc(pointer, newsize); }

void (*_free_spoof)(void *pointer, _def_info_vars) = _free_adapt;
void *(*_malloc_spoof)(size_t size, _def_info_vars) = _malloc_adapt;
void *(*_calloc_spoof)(size_t number_of, size_t size, _def_info_vars) = _calloc_adapt;
void *(*_realloc_spoof)(void *pointer, size_t newsize, _def_info_vars) = _realloc_adapt;

void start__memcheck()
{
    _free_spoof = _free_marked;
    _malloc_spoof = _malloc_marked;
    _calloc_spoof = _calloc_marked;
    _realloc_spoof = _realloc_marked;
}

#define free(pointer) _free_spoof(pointer, _get_info_vars)
#define malloc(pointer) _malloc_spoof(pointer, _get_info_vars)
#define calloc(number_of, size) _calloc_spoof(number_of, size, _get_info_vars)
#define realloc(pointer, newsize) _realloc_spoof(pointer, newsize, _get_info_vars)