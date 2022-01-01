#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>
#define FLAT_INCLUDES
#include "../../range/def.h"
#include "../../window/def.h"
#include "../../window/alloc.h"
#include "../../convert/source.h"
#include "../error/error.h"
#include "../../immutable/immutable.h"
#include "../tree/tree.h"
#include "transform.h"

bool lang_transform_iterate (window_lang_transform_p * stack)
{
    lang_transform ** top = stack->region.end - 1;

    if (top < stack->region.begin)
    {
	return false;
    }

    return (*top)->iter(stack, *top);
}

lang_transform * lang_transform_new (window_lang_transform_p * stack, size_t extra)
{
    return *window_push (*stack) = calloc (1, sizeof(lang_transform) + extra);
}
