#ifndef FLAT_INCLUDES
#include <stddef.h>
#include <stdbool.h>
#define FLAT_INCLUDES
#include "../../range/def.h"
#include "../../window/def.h"
#include "../../convert/def.h"
#include "../error/error.h"
#include "../../immutable/immutable.h"
#include "../tree/tree.h"
#endif

typedef convert_interface * (*lang_preprocessor_accessor)(const char * path, void * arg);
lang_tree_node * lang_preprocessor_load (const char * path, lang_preprocessor_accessor accessor, void * arg);
