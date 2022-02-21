#include <stddef.h>
#include <stdbool.h>
#include <assert.h>
#include <unistd.h>
#define FLAT_INCLUDES
#include "../../../range/def.h"
#include "../../../window/def.h"
#include "../../../window/alloc.h"
#include "../../../convert/source.h"
#include "../../../keyargs/keyargs.h"
#include "../../../convert/fd/source.h"
#include "../../error/error.h"
#include "../tokenizer.h"
#include "../../../log/log.h"

int main()
{
    window_unsigned_char buffer = {0};
    fd_source fd_read = fd_source_init (STDIN_FILENO, &buffer);
    lang_tokenizer_state state = { .input_position.line = 1 };

    bool error = false;
    range_const_char token;
    
    while (lang_tokenizer_read_source (&error, &token, &state, &fd_read.source))
    {
	log_normal ("TOKEN: (%02d,%02d) [%.*s]", state.token_position.line, state.token_position.col, range_count(token), token.begin);
    }

    window_clear (buffer);
    
    if (error)
    {
	log_fatal ("An error occurred");
    }

    return 0;
    
fail:
    return 1;
}
