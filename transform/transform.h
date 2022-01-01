#ifndef FLAT_INCLUDES
#include <stddef.h>
#include <stdbool.h>
#define FLAT_INCLUDES
#include "../../range/def.h"
#include "../../window/def.h"
#include "../../convert/source.h"
#include "../error/error.h"
#include "../../immutable/immutable.h"
#include "../tree/tree.h"
#endif

typedef struct lang_transform lang_transform;
range_typedef(lang_transform*, lang_transform_p);
window_typedef(lang_transform*, lang_transform_p);
typedef bool (*lang_transform_check)(lang_tree_node * root);
typedef bool (*lang_transform_iter)(window_lang_transform_p * stack, lang_transform * top);
struct lang_transform {
    lang_tree_node ** retval;
    lang_transform_iter iter;
};
