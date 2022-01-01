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
    range_const_char dirname;
    lang_tree_node * root;
}
    pp_file;

#ifndef PATH_SEPARATOR
#define PATH_SEPARATOR '/'
#endif
