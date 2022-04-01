#include "../tree.h"
#include "../../../convert/fd/source.h"
#include <unistd.h>
#include <assert.h>
#include "../../../window/alloc.h"
#include "../../../log/log.h"

int main()
{
    window_unsigned_char read_buffer = {0};
    fd_source fd_read = fd_source_init (STDIN_FILENO, &read_buffer);

    string_table table = {0};

    lang_tree_node * root;

    if (!lang_tree_load_source (&root, &table, &fd_read.source))
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
