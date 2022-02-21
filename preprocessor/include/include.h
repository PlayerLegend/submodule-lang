#ifndef FLAT_INCLUDES
#include <stddef.h>
#include <stdbool.h>
#define FLAT_INCLUDES
#include "../../../range/def.h"
#include "../../../window/def.h"
#include "../../../convert/source.h"
#include "../../error/error.h"
#include "../../../immutable/immutable.h"
#include "../../../immutable/window.h"
#include "../../tree/tree.h"
#include "../../transform/transform.h"
#endif

typedef struct {
    const lang_tree_node * arglist;
}
    transform_include;

bool transform_include_new (bool * error, lang_tree_node ** result, lang_transform_state * state, const lang_tree_node * arglist);

