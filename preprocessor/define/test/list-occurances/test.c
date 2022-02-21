#include <stdint.h>
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

int main(int argc, char * argv[])
{
    assert (argc > 1);

    host_string_to_none_table table = host_string_to_none_table_initializer;
        
    window_lang_tree_node_p occurances = {0};
    
    lang_tree_node * occurances_root = load_tree (&table);

    table_string_query * match = &host_string_to_none_include_string(&table, argv[1])->query;

    lang_define_arg_list_occurances (&occurances, occurances_root, match);

    lang_tree_node ** occurance;

    for_range (occurance, occurances.region)
    {
	assert (*occurance);
	
	log_normal ("%d %d", (*occurance)->source_position.line, (*occurance)->source_position.col);
    }

    window_clear (occurances);

    lang_tree_free (occurances_root);

    return 0;
}
