#include <stddef.h>
#include <stdbool.h>
#include <assert.h>
#include <stdint.h>
#define FLAT_INCLUDES
#include "../../../../range/def.h"
#include "../../../../window/def.h"
#include "../../../../convert/source.h"
#include "../../../error/error.h"
#include "../../../../table/string.h"
#include "../../../tree/tree.h"
#include "../../transform.h"
#include "../copy.h"

int main(int argc, char * argv[])
{
    lang_transform_state state = {0};

    lang_tree_node * result = NULL;

    bool error = false;

    assert(argc == 2);

    host_string_to_none_table table = {0};
    
    lang_tree_node * origin = lang_tree_load_path(&error, &table, argv[1]);

    lang_tree_print (origin);
    
    assert (!error);

    assert (origin);

    assert (transform_copy_new (&error, &result, &state, origin));

    assert (lang_transform_execute (&state));

    assert (result);

    lang_tree_free (origin);

    lang_tree_print (result);

    lang_tree_free (result);

    lang_transform_state_clear (&state);

    return 0;
}
