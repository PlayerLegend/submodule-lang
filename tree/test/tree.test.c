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
    lang_tokenizer_state state = { .input_position.line = 1, .source = &fd_read.source };
    range_const_char token_text;
    immutable_text token_immutable;
   
    bool error = false;
    lang_tree_build_env env = {0};

    lang_tree_build_start(&env);
    
    while (tokenizer_read (&error, &token_text, &state))
    {
	token_immutable = immutable_string_range(NULL, &token_text);

	assert (token_immutable.text);
	
	//log_debug ("added '%s'", token_immutable.text);
	lang_tree_build_update(&env, &state.token_position, token_immutable);
	//log_normal ("TOKEN: (%02d,%02d) [%.*s]", state.token_position.line, state.token_position.col, range_count(state.token_contents.region), state.token_contents.region.begin);
    }

    if (error)
    {
	log_fatal ("A tokenizer error occurred");
    }

    window_clear (read_buffer);

    lang_tree_node * root = lang_tree_build_finish(&env);
    
    if (!root)
    {
	log_fatal ("A tree error occurred");
    }
    
    lang_tree_build_clear (&env);

    lang_tree_node * copy_root = lang_tree_copy (root);
    
    lang_tree_free (root);

    lang_tree_print (copy_root);

    lang_tree_free (copy_root);

    return 0;
    
fail:
    return 1;
}
