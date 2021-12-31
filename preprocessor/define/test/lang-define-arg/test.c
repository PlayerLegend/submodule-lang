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
#include "../../../../../immutable/immutable.h"
#include "../../../../tree/tree.h"
#include "../../define.h"
#include "../load.h"

int main(int argc, char * argv[])
{
    assert (argc > 1);
    
    immutable_namespace * namespace = immutable_namespace_new();
    
    lang_tree_node * load_root = load_tree (namespace);
    lang_tree_node * occurances_root;

    assert (lang_tree_get_child(&occurances_root, load_root));

    lang_tree_node * replace = load_root->peer;

    assert (replace);

    immutable_text match = immutable_string(namespace, argv[1]);
    
    lang_define_arg arg = {0};
    window_lang_define_arg_occurance occurance_buffer = {0};

    lang_define_arg_init (&arg, &occurance_buffer, &occurances_root, match);

    lang_define_arg_set (&arg, replace);
    
    lang_tree_print(occurances_root);

    lang_tree_free (load_root);

    lang_define_arg_clear (&arg);

    immutable_namespace_free(namespace);

    return 0;
}
