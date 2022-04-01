#include "../tokenizer.h"
#include "../../../convert/fd/source.h"
#include <unistd.h>
#include <assert.h>
#include "../../../window/alloc.h"
#include "../../../log/log.h"

int main()
{
    window_unsigned_char buffer = {0};
    fd_source fd_read = fd_source_init (STDIN_FILENO, &buffer);
    lang_tokenizer_state state = { .input_position.line = 1 };

    status status;

    while ( (status = lang_tokenizer_read_source(&state, &fd_read.source)) == STATUS_UPDATE )
    {
	log_normal ("TOKEN: (%02d,%02d) [" RANGE_FORMSPEC "]", state.token_position.line, state.token_position.col, RANGE_FORMSPEC_ARG(state.text));
    }

    window_clear (buffer);

    assert (status == STATUS_END);

    return 0;
}
