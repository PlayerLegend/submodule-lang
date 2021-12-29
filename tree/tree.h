#ifndef FLAT_INCLUDES
#include <stddef.h>
#include <stdbool.h>
#define FLAT_INCLUDES
#include "../../range/def.h"
#include "../../window/def.h"
#include "../../convert/source.h"
#include "../error/error.h"
#include "../../immutable/immutable.h"
#endif

typedef struct lang_tree_node lang_tree_node;

struct lang_tree_node {
    bool is_text;
    bool is_quoted;
    union {
	immutable_text immutable;
	lang_tree_node * child;
    };
    lang_token_position source_position;
    lang_tree_node * peer;
};

range_typedef(lang_tree_node*,lang_tree_node_p);
window_typedef(lang_tree_node*,lang_tree_node_p);

range_typedef(lang_tree_node**,lang_tree_node_pp);
window_typedef(lang_tree_node**,lang_tree_node_pp);

typedef struct {
    lang_tree_node * root;
    window_lang_tree_node_pp stack;
    lang_token_position last_position;
    immutable_namespace * namespace;
}
    lang_tree_build_env;

void lang_tree_build_start (lang_tree_build_env * env, immutable_namespace * namespace);
bool lang_tree_build_update (lang_tree_build_env * env, const lang_token_position * token_position, const range_const_char * token);
lang_tree_node * lang_tree_build_finish (lang_tree_build_env * env);
lang_tree_node * lang_tree_load (immutable_namespace * namespace, convert_source * source);
void lang_tree_build_clear (lang_tree_build_env * env);
void lang_tree_free (lang_tree_node * root);
void lang_tree_print (lang_tree_node * root);
lang_tree_node * lang_tree_copy (lang_tree_node * root);
