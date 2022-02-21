#ifndef FLAT_INCLUDES
#include <stdint.h>
#include <stddef.h>
#define FLAT_INCLUDES
#include "../../../range/def.h"
#include "../../../table/string.h"
#endif

typedef struct {
    table_string_query * define;
    table_string_query * include;
}
    pp_keywords;

inline static void pp_keywords_init (pp_keywords * target, host_string_to_none_table * table)
{
    target->define = &host_string_to_none_include_string(table, "define")->query;
    target->include = &host_string_to_none_include_string(table, "include")->query;
}
