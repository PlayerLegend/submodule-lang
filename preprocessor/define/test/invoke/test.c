#include <stddef.h>
#include <stdbool.h>
#include <assert.h>
#include <unistd.h>
#define FLAT_INCLUDES
#include "../../../../../range/def.h"
#include "../../../../../window/def.h"
#include "../../../../../window/alloc.h"
#include "../../../../../keyargs/keyargs.h"
#include "../../../../../convert/source.h"
#include "../../../../../convert/fd/source.h"
#include "../../../../../log/log.h"
#include "../../../../error/error.h"
#include "../../../../../table/string.h"
#include "../../../../tree/tree.h"
#include "../../define.h"
#include "../load.h"

int main()
{
    host_string_to_none_table table = { .link_size = table_string_link_size(table) };
    
    lang_tree_node * load_root = load_tree (&table);
    lang_tree_node * occurances_root;

    assert (lang_tree_get_child(&occurances_root, load_root));

    lang_tree_node * replace = load_root->peer;

    assert (replace);

    lang_define define = {0};
    window_lang_define_arg arg_buffer = {0};
    window_lang_tree_node_p occurance_buffer = {0};

    lang_define_init (&define, &arg_buffer, &occurance_buffer, load_root);

    bool error = false;

    lang_tree_free(lang_define_invoke (&error, &define, load_root->peer));
    lang_tree_free(lang_define_invoke (&error, &define, load_root->peer));

    lang_tree_node * instance = lang_define_invoke (&error, &define, load_root->peer);

    window_clear (arg_buffer);
    window_clear (occurance_buffer);
    
    lang_tree_print(instance);

    lang_tree_free (load_root);
    lang_tree_free (instance);

    lang_define_clear (&define);

    return 0;
}
