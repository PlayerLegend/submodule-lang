#include <stddef.h>
#include <stdbool.h>
#define FLAT_INCLUDES
#include "../../../range/def.h"
#include "../../../window/def.h"
#include "../../../convert/source.h"
#include "../../error/error.h"
#include "../../../immutable/immutable.h"
#include "../../tree/tree.h"
#include "../../transform/transform.h"

bool transform_recurse_iter(lang_transform_state * state, lang_transform * top)
{
    
}

void transform_recurse_clear(lang_transform_state * state, lang_transform * target)
{

}

bool transform_recurse_new (bool * error, lang_tree_node ** result, lang_transform_state * state)
{
    lang_transform * new_transform = lang_transform_alloc (&state->stack, 0);
    new_transform->iter = transform_recurse_iter;
    new_transform->clear = transform_recurse_clear;
    
}
