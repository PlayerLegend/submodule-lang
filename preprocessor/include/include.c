#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <assert.h>
#define FLAT_INCLUDES
#include "../../../range/def.h"
#include "../../../window/def.h"
#include "../../../window/alloc.h"
#include "../../../convert/source.h"
#include "../../error/error.h"
#include "../../../table/string.h"
#include "../../tree/tree.h"
#include "../../transform/transform.h"
#include "include.h"
#include "../keywords/keywords.h"
#include "../global/global.h"
#include "../file/file.h"

bool transform_include_iter(lang_transform_state * state, lang_transform * top)
{
    transform_include * arg = lang_transform_arg(top, transform_include);

    if (arg->arglist)
    {
	while (*top->result)
	{
	    top->result = &(*top->result)->peer;
	}

	if (!arg->arglist->is_text)
	{
	    return false;
	}

	bool error = false;

	if (!transform_file_new(&error, top->result, state, arg->arglist->ref))
	{
	    return false;
	}

	arg->arglist = arg->arglist->peer;
    }
    else
    {
	lang_transform_state_pop (state);
    }

    return true;
}

bool transform_include_new (bool * error, lang_tree_node ** result, lang_transform_state * state, const lang_tree_node * arglist)
{
    lang_transform * new_transform = lang_transform_alloc(&state->stack, sizeof(transform_include));

    new_transform->iter = transform_include_iter;
    new_transform->result = result;

    lang_transform_arg(new_transform, transform_include)->arglist = arglist;

    return true;
}
