#ifndef FLAT_INCLUDES
#include <stddef.h>
#include <stdbool.h>
#define FLAT_INCLUDES
#include "../../../range/def.h"
#include "../../../window/def.h"
#include "../../../convert/source.h"
#include "../../error/error.h"
#include "../../../immutable/immutable.h"
#include "../../tree/tree.h"
#include "../transform.h"
#endif

typedef struct {
    const lang_tree_node * target;
}
    transform_copy;

bool transform_copy_new (bool * error, lang_tree_node ** result, lang_transform_state * state, const lang_tree_node * root);
bool transform_copy_iter(lang_transform_state * state, lang_transform * top);

