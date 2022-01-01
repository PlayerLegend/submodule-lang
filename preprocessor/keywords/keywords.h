#ifndef FLAT_INCLUDES
#define FLAT_INCLUDES
#include "../../../range/def.h"
#include "../../../immutable/immutable.h"
#endif

typedef struct {
    immutable_text define;
    immutable_text include;
}
    pp_keywords;

inline static void pp_keywords_init (pp_keywords * target, immutable_namespace * namespace)
{
    target->define = immutable_string (namespace, "define");
    target->include = immutable_string (namespace, "include");
}
