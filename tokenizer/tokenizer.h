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
    convert_source * source;
}
    lang_tokenizer_state;

bool tokenizer_read (bool * error, range_const_char * result, lang_tokenizer_state * state);
