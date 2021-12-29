#ifndef FLAT_INCLUDES
#include <stddef.h>
#include <stdbool.h>
#define FLAT_INCLUDES
#include "../../range/def.h"
#include "../../window/def.h"
#include "../../window/alloc.h"
#include "../error/error.h"
#include "../../immutable/immutable.h"
#include "tree.h"
#endif

inline static void lang_tree_stack_push (window_lang_tree_node_p * stack, lang_tree_node * node)
{
    if (node)
    {
	*window_push (*stack) = node;
    }
}

inline static lang_tree_node * lang_tree_stack_pop (window_lang_tree_node_p * stack)
{
    if (range_is_empty(stack->region))
    {
	return NULL;
    }

    lang_tree_node * retval = stack->region.end[-1];

    stack->region.end--;

    return retval;
}
