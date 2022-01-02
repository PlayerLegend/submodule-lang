#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>
#include <assert.h>
#define FLAT_INCLUDES
#include "../../range/def.h"
#include "../../window/def.h"
#include "../../window/alloc.h"
#include "../../convert/source.h"
#include "../error/error.h"
#include "../../immutable/immutable.h"
#include "../tree/tree.h"
#include "transform.h"
#include "../../log/log.h"

bool lang_transform_execute (lang_transform_state * state)
{
    lang_transform * top;
    
    while (!range_is_empty(state->stack.region))
    {
	top = state->stack.region.end[-1];

	if (!top->iter(state, top))
	{
	    while (!range_is_empty (state->stack.region))
	    {
		lang_transform_state_pop (state);
	    }
	    
	    return false;
	}
    }

    return true;
}

lang_transform * lang_transform_alloc (window_lang_transform_p * stack, size_t extra)
{
    return *window_push(*stack) = calloc (1, sizeof(lang_transform) + extra);
}

void lang_transform_state_pop (lang_transform_state * state)
{
    assert (!range_is_empty (state->stack.region));

    state->stack.region.end--;

    lang_transform * pop = *state->stack.region.end;

    if (pop->clear)
    {
	pop->clear (state, pop);
    }
    
    free (pop);
}

void lang_transform_state_clear (lang_transform_state * state)
{
    while (!range_is_empty (state->stack.region))
    {
	lang_transform_state_pop (state);
    }

    window_clear (state->stack);
}
