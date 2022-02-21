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
#include "../../transform/transform.h"
#endif

typedef struct {
    range_const_char dirname;
    lang_tree_node * root;
}
    transform_file;

bool transform_file_new (bool * error, lang_tree_node ** result, lang_transform_state * state, const table_string_query * path);
void transform_file_pwd (window_char * result, const range_const_lang_transform_p * stack);
bool transform_file_iter(lang_transform_state * state, lang_transform * top);
void transform_file_clear(lang_transform_state * state, lang_transform * target);

#ifndef PATH_SEPARATOR
#define PATH_SEPARATOR '/'
#endif
