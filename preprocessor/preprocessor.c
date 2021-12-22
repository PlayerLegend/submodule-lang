#include <stddef.h>
#include <stdbool.h>
#define FLAT_INCLUDES
#include "../../range/def.h"
#include "../../window/def.h"
#include "../../convert/def.h"
#include "../error/error.h"
#include "../../immutable/immutable.h"
#include "../tree/tree.h"
#include "preprocessor.h"

range_typedef (char**, char_pp);

typedef struct {
    const char * name;
    range_const_char_pp occurances;
}
    lang_define_arg;

range_typedef (lang_define_arg, lang_define_arg);

typedef struct {
    lang_tree_node * root;
    range_lang_define_arg args;
}
    lang_define;

range_typedef(lang_define, lang_define);
window_typedef(lang_define, lang_define);

typedef struct {
    window_lang_define defines;
    bool is_loading_finished;
}
    lang_preprocessor_unit;

typedef struct {
}
    lang_preprocessor_env;

#define table_string_value lang_preprocessor_unit unit

#include "../../table/table-string.h"

lang_tree_node * lang_preprocessor_load (const char * uri)
{
    
}
