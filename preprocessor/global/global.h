#ifndef FLAT_INCLUDES
#include <stdint.h>
#include <stddef.h>
#define FLAT_INCLUDES
#include "../../../range/def.h"
#include "../../../table/string.h"
#include "../keywords/keywords.h"
#endif

typedef struct {
    host_string_to_none_table * table;
    pp_keywords keywords;
}
    transform_global;
    
