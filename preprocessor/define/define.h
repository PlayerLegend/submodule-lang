#ifndef FLAT_INCLUDES
#include <stddef.h>
#include <stdbool.h>
#define FLAT_INCLUDES
#include "../../../range/def.h"
#include "../../../window/def.h"
#include "../../../convert/source.h"
#include "../../error/error.h"
#include "../../../immutable/immutable.h"
#include "../../tree/tree.h"
#endif

typedef struct {
    lang_tree_node ** in;
    lang_tree_node ** out;
}
    lang_define_arg_occurance;

range_typedef(lang_define_arg_occurance, lang_define_arg_occurance);
window_typedef(lang_define_arg_occurance, lang_define_arg_occurance);

typedef struct {
    immutable_text name;
    lang_tree_node * default_value;
    range_lang_define_arg_occurance occurances;
    bool is_set;
}
    lang_define_arg;

void lang_define_arg_clear (lang_define_arg * target);

range_typedef (lang_define_arg, lang_define_arg);
window_typedef (lang_define_arg, lang_define_arg);

typedef struct {
    immutable_text name;
    lang_tree_node * root;
    range_lang_define_arg args;
}
    lang_define;

void lang_define_clear (lang_define * target);
void lang_define_arg_list_occurances (window_lang_define_arg_occurance * occurances, lang_tree_node ** root, immutable_text match);
void lang_define_arg_init (lang_define_arg * target, window_lang_define_arg_occurance * buffer, lang_tree_node ** root, immutable_text match);
bool lang_define_init (lang_define * target, window_lang_define_arg * arg_buffer, window_lang_define_arg_occurance * occurance_buffer, lang_tree_node * root);
void lang_define_arg_occurance_set (lang_define_arg_occurance * occurance, const lang_tree_node * node);
void lang_define_arg_set (lang_define_arg * arg, const lang_tree_node * node);
bool lang_define_arg_is_set (lang_define_arg * arg);
