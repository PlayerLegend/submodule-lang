#ifndef FLAT_INCLUDES
#define FLAT_INCLUDES
#include "../../../range/def.h"
#include "../../../immutable/immutable.h"
#include "../keywords/keywords.h"
#endif

typedef struct {
    immutable_namespace * namespace;
    pp_keywords keywords;
}
    transform_global;
    
