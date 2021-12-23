#include <assert.h>
#include <stddef.h>
#include <stdbool.h>
#include <ctype.h>
#include <stdlib.h>
#define FLAT_INCLUDES
#include "../../range/def.h"
#include "../../table/table.h"
#include "../../table/table-string.h"
#include "../../window/def.h"
#include "../../window/alloc.h"
#include "../../convert/source.h"
#include "../../log/log.h"
#include "../error/error.h"
#include "tokenizer.h"

static bool get_token_size (size_t * size, bool * quote, const range_const_unsigned_char * input)
{
    const char * end = input->char_cast.const_cast.begin + *size;

    bool paren_terminate = false;

    while (end < input->char_cast.const_cast.end)
    {
	if (*end == '"')
	{
	    *quote = !*quote;
	}
	else if (!*quote)
	{
	    if (isspace (*end))
	    {
		break;
	    }

	    if (*end == '(' || *end == ')')
	    {
		if (end == input->char_cast.const_cast.begin)
		{
		    end++;
		}

		paren_terminate = true;
		
		break;
	    }
	}
	    
	end++;
    }
    
    *size = range_index (end, input->char_cast.const_cast);

    return paren_terminate || *size < (size_t) range_count (*input);
}

static void update_position (lang_token_position * position, char c)
{
    if (c == '\n')
    {
	position->line++;
	position->col = 0;
    }
    else
    {
	position->col++;	
    }
}

static bool skip_whitespace (bool * error, lang_tokenizer_state * state)
{
    window_unsigned_char * buffer = state->source->contents;
    
    while (true)
    {
	while (buffer->region.begin < buffer->region.end && isspace (*buffer->region.begin))
	{
	    update_position (&state->input_position, *buffer->region.begin);
	    buffer->region.begin++;
	}

	if (range_is_empty (buffer->region))
	{
	    if (!convert_fill(error, state->source))
	    {
		return false;
	    }
	}
	else
	{
	    break;
	}
    }

    assert (!range_is_empty (buffer->region));
    assert (!isspace (*buffer->region.begin));

    return true;
}

bool tokenizer_read (bool * error, range_const_char * result, lang_tokenizer_state * state)
{
    window_unsigned_char * buffer = state->source->contents;
    
    window_alloc (*buffer, 1024);

    if (!skip_whitespace (error, state))
    {
	return false;
    }
    
    size_t size = 0;
    bool quoted = false;

    state->token_position = state->input_position;
    
    while (!get_token_size(&size, &quoted, &buffer->region.const_cast))
    {
	if (!convert_grow (error, state->source, 64))
	{
	    break;
	}
    }

    if (*error)
    {
	return false;
    }
    
    result->begin = (const char*) buffer->region.begin;
    result->end = result->begin + size;

    while ((const char*) buffer->region.begin < result->end)
    {
	update_position (&state->input_position, *buffer->region.begin);
	buffer->region.begin++;
    }

    assert (!range_is_empty (*result));
    
    return !*error;
}
