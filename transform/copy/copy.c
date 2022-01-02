#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>
#define FLAT_INCLUDES
#include "../../../range/def.h"
#include "../../../window/def.h"
#include "../../../convert/source.h"
#include "../../error/error.h"
#include "../../../immutable/immutable.h"
#include "../../tree/tree.h"
#include "../transform.h"
#include "copy.h"
#include "../../../log/log.h"

bool transform_copy_new (bool * error, lang_tree_node ** result, lang_transform_state * state, const lang_tree_node * root)
{
    lang_transform * new_transform = lang_transform_alloc(&state->stack, sizeof(transform_copy));

    new_transform->iter = transform_copy_iter;

    new_transform->result = result;

    lang_transform_arg(new_transform, transform_copy)->target = root;

    return true;
}

bool transform_copy_iter(lang_transform_state * state, lang_transform * top)
{
    transform_copy * arg = lang_transform_arg(top, transform_copy);

    if (arg->target)
    {
	lang_tree_node * new = *top->result = calloc (1, sizeof(lang_tree_node));

	*new = *arg->target;

	if (!new->is_text && new->child)
	{
	    bool error = false;
	    
	    if (!transform_copy_new (&error, &new->child, state, new->child))
	    {
		return false;
	    }
	}

	arg->target = new->peer;
	top->result = &new->peer;
    }
    else
    {
	lang_transform_state_pop(state);
    }

    return true;
}
