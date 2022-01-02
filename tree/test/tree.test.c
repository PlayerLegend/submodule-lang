#include <stddef.h>
#include <stdbool.h>
#include <assert.h>
#include <unistd.h>
#define FLAT_INCLUDES
#include "../../../range/def.h"
#include "../../../log/log.h"
#include "../../../window/def.h"
#include "../../../window/alloc.h"
#include "../../../keyargs/keyargs.h"
#include "../../../convert/source.h"
#include "../../../convert/fd/source.h"
#include "../../error/error.h"
#include "../../../immutable/immutable.h"
#include "../tree.h"
#include "../../tokenizer/tokenizer.h"

int main()
{
    window_unsigned_char read_buffer = {0};
    fd_source fd_read = fd_source_init (.fd = STDIN_FILENO, .contents = &read_buffer);

    bool error = false;

    lang_tree_node * root = lang_tree_load_source (&error, NULL, &fd_read.source);
    
    if (!root)
    {
	log_fatal ("A tree error occurred");
    }
    
    lang_tree_node * copy_root = lang_tree_copy (root);
    
    lang_tree_free (root);

    lang_tree_print (copy_root);

    lang_tree_free (copy_root);

    window_clear (read_buffer);

    return 0;
    
fail:
    return 1;
}
