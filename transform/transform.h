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
typedef struct lang_transform_state lang_transform_state;
range_typedef(lang_transform*, lang_transform_p);
window_typedef(lang_transform*, lang_transform_p);
typedef bool (*lang_transform_iter)(lang_transform_state * state, lang_transform * top);
typedef void (*lang_transform_clear)(lang_transform_state * state, lang_transform * target);

struct lang_transform {
    lang_tree_node ** result;
    lang_transform_iter iter;
    lang_transform_clear clear;
};

struct lang_transform_state {
    window_lang_transform_p stack;
    void * global;
};

bool lang_transform_execute (lang_transform_state * state);
lang_transform * lang_transform_alloc (window_lang_transform_p * stack, size_t extra);

inline static void * _lang_transform_arg(lang_transform * transform)
{
    return transform + 1;
}

#define lang_transform_arg(transform, type)	\
    ( (type*) _lang_transform_arg (transform) )

inline static const void * _lang_transform_arg_const(const lang_transform * transform)
{
    return transform + 1;
}

#define lang_transform_arg_const(transform, type)	\
    ( (const type*) _lang_transform_arg_const (transform) )

void lang_transform_state_pop (lang_transform_state * state);
void lang_transform_state_clear (lang_transform_state * state);
