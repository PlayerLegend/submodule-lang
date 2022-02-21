#ifndef FLAT_INCLUDES
#include <stddef.h>
#include <stdbool.h>
#define FLAT_INCLUDES
#include "../../../range/def.h"
#include "../../../window/def.h"
#include "../../../convert/source.h"
#include "../../error/error.h"
#include "../../../table/string.h"
#include "../../tree/tree.h"
#endif

typedef struct {
    const table_string_query * name;
    lang_tree_node * default_value;
    range_lang_tree_node_p occurances;
    bool is_set;
}
    lang_define_arg;

void lang_define_arg_clear (lang_define_arg * target);

range_typedef (lang_define_arg, lang_define_arg);
window_typedef (lang_define_arg, lang_define_arg);

typedef struct {
    const table_string_query * name;
    lang_tree_node * root;
    range_lang_define_arg args;
}
    lang_define;

range_typedef (lang_define, lang_define);
window_typedef (lang_define, lang_define);

void lang_define_clear (lang_define * target);
void lang_define_arg_list_occurances (window_lang_tree_node_p * occurances, lang_tree_node * root, const table_string_query * match);
void lang_define_arg_init (lang_define_arg * target, window_lang_tree_node_p * buffer, lang_tree_node * root, const table_string_query * match);
bool lang_define_init (lang_define * target, window_lang_define_arg * arg_buffer, window_lang_tree_node_p * occurance_buffer, lang_tree_node * root);
void lang_define_arg_occurance_set (lang_tree_node * occurance, const lang_tree_node * node);
void lang_define_arg_set (lang_define_arg * arg, const lang_tree_node * node);
bool lang_define_arg_is_set (lang_define_arg * arg);
lang_tree_node * lang_define_invoke (bool * error, lang_define * define, const lang_tree_node * args);
