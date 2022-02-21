#ifndef FLAT_INCLUDES
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#define FLAT_INCLUDES
#include "../../range/def.h"
#include "../../window/def.h"
#include "../../convert/source.h"
#include "../error/error.h"
#include "../../table/string.h"
#endif

typedef struct lang_tree_node lang_tree_node;

struct lang_tree_node {
    bool is_text;
    union {
	const string_pair * ref;
	lang_tree_node * child;
    };
    lang_token_position source_position;
    lang_tree_node * peer;
    string_table * table;
};

range_typedef(lang_tree_node*,lang_tree_node_p);
window_typedef(lang_tree_node*,lang_tree_node_p);

range_typedef(lang_tree_node**,lang_tree_node_pp);
window_typedef(lang_tree_node**,lang_tree_node_pp);

typedef struct {
    lang_tree_node * root;
    window_lang_tree_node_pp stack;
    lang_token_position last_position;
    string_table * table;
}
    lang_tree_build_env;

void lang_tree_build_start (lang_tree_build_env * env, string_table * table);
bool lang_tree_build_update (lang_tree_build_env * env, const lang_token_position * token_position, const range_const_char * token_text);
lang_tree_node * lang_tree_build_finish (bool * error, lang_tree_build_env * env);
lang_tree_node * lang_tree_load_source (bool * error, string_table * table, convert_source * source);
lang_tree_node * lang_tree_load_path (bool * error, string_table * table, const char * path);
lang_tree_node * lang_tree_load_mem (bool * error, string_table * table, range_const_unsigned_char * mem);
void lang_tree_build_clear (lang_tree_build_env * env);
void lang_tree_free (lang_tree_node * root);
void lang_tree_print (lang_tree_node * root);
lang_tree_node * lang_tree_copy (const lang_tree_node * root);

inline static bool lang_tree_get_child (lang_tree_node ** result, lang_tree_node * parent)
{
    if (!parent || parent->is_text)
    {
	return false;
    }

    *result = parent->child;

    return true;
}

inline static bool lang_tree_get_text (const string_pair ** ref, lang_tree_node * parent)
{
    if (!parent || !parent->is_text)
    {
	return false;
    }

    *ref = parent->ref;

    return true;
}

lang_tree_node * lang_tree_get_option_by_ref (bool * error, lang_tree_node * node, string_pair * match);

inline static lang_tree_node * lang_tree_get_option_by_string (bool * error, lang_tree_node * node, const char * match)
{
    return lang_tree_get_option_by_ref(error, node, string_include_string(node->table, match));
}

inline static lang_tree_node * lang_tree_get_option_by_range (bool * error, lang_tree_node * node, const range_const_char * match)
{
    return lang_tree_get_option_by_ref(error, node, string_include_range(node->table, match));
}
