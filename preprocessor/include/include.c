
#include <stddef.h>
#include <stdbool.h>
#define FLAT_INCLUDES
#include "../../../range/def.h"
#include "../../../window/def.h"
#include "../../../window/alloc.h"
#include "../../../convert/source.h"
#include "../../error/error.h"
#include "../../../immutable/immutable.h"
#include "../../../immutable/window.h"
#include "../../tree/tree.h"
#include "../../transform/transform.h"
#include "include.h"
#include "../keywords/keywords.h"
#include "../global/global.h"
#include "../file/file.h"

bool transform_include_new (bool * error, lang_tree_node ** result, lang_transform_state * state, const lang_tree_node * root)
{
    if (root->is_text || !root->child)
    {
	return false;
    }

    lang_tree_node * keyword = root->child;

    transform_global * global = state->global;

    if (!keyword->is_text || keyword->immutable.text != global->keywords.include.text)
    {
	return false;
    }

    lang_transform * new_transform = lang_transform_alloc(&state->stack, sizeof(transform_include));

    new_transform->iter = transform_include_iter;
    new_transform->result = result;

    lang_transform_arg(new_transform, transform_include)->args = keyword->peer;

    return true;
}

bool transform_include_iter(lang_transform_state * state, lang_transform * top)
{
    transform_include * arg = lang_transform_arg(top, transform_include);

    if (arg->args)
    {
	if (!arg->args->is_text)
	{
	    return false;
	}

	while (*top->result)
	{
	    top->result = &(*top->result)->peer;
	}

	bool error = false;

	if (!transform_file_new(&error, top->result, state, arg->args) || error)
	{
	    return false;
	}

	arg->args = arg->args->peer;
    }
    else
    {
	lang_transform_pop (state);
    }

    return true;
}

