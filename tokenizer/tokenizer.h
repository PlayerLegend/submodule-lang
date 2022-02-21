#ifndef FLAT_INCLUDES
#include <stddef.h>
#include <stdbool.h>
#define FLAT_INCLUDES
#include "../../range/def.h"
#include "../../window/def.h"
#include "../../convert/source.h"
#include "../error/error.h"
#endif

typedef struct {
    lang_token_position input_position;
    lang_token_position token_position;
}
    lang_tokenizer_state;

bool lang_token_scan (lang_tokenizer_state * state, range_const_unsigned_char * text);

bool lang_tokenizer_read_mem (bool * error, range_const_char * result, lang_tokenizer_state * state, range_const_unsigned_char * mem);
bool lang_tokenizer_read_source (bool * error, range_const_char * result, lang_tokenizer_state * state, convert_source * source);

