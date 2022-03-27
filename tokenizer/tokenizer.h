#ifndef FLAT_INCLUDES
#include "../../convert/source.h"
#include "../error/error.h"
#endif

typedef struct {
    lang_token_position input_position;
    lang_token_position token_position;
    range_const_char text;
}
    lang_tokenizer_state;

status lang_tokenizer_read_mem (lang_tokenizer_state * state, range_const_char * text);
status lang_tokenizer_read_source (lang_tokenizer_state * state, convert_source * source);

