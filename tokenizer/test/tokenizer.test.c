#include <stddef.h>
#include <stdbool.h>
#include <assert.h>
#define FLAT_INCLUDES
#include "../../../range/def.h"
#include "../../../window/def.h"
#include "../../../window/alloc.h"
#include "../../../convert/def.h"
#include "../../../keyargs/keyargs.h"
#include "../../../convert/fd.h"
#include "../../error/error.h"
#include "../tokenizer.h"
#include "../../../log/log.h"

int main()
{
    window_unsigned_char buffer = {0};
    fd_interface fd_read = fd_interface_init (.fd = STDIN_FILENO, .read_buffer = &buffer);
    lang_tokenizer_state state = { .input_position.line = 1, .source = &fd_read.interface };

    bool error = false;
    range_const_char token;
    
    while (tokenizer_read (&error, &token, &state))
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
