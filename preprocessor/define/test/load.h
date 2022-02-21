#ifndef FLAT_INCLUDES
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <assert.h>
#include <unistd.h>
#define FLAT_INCLUDES
#include "../../../../range/def.h"
#include "../../../../window/def.h"
#include "../../../../window/alloc.h"
#include "../../../../keyargs/keyargs.h"
#include "../../../../convert/source.h"
#include "../../../../convert/fd/source.h"
#include "../../../../log/log.h"
#include "../../../error/error.h"
#include "../../../../table/string.h"
#include "../../../tree/tree.h"
#include "../define.h"
#endif

static lang_tree_node * load_tree (host_string_to_none_table * namespace)
{
    window_unsigned_char contents = {0};
    
    fd_source fd_source = fd_source_init (.fd = STDIN_FILENO, .contents = &contents);

    bool error = false;

    lang_tree_node * retval = lang_tree_load_source (&error, namespace, &fd_source.source);

    assert (!error);
    
    assert (retval);

    window_clear (contents);

    convert_source_clear (&fd_source.source);

    return retval;
}
