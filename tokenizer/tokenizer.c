#include <stdint.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>
#include <ctype.h>
#include <stdlib.h>
#define FLAT_INCLUDES
#include "../../range/def.h"
#include "../../window/def.h"
#include "../../window/alloc.h"
#include "../../convert/source.h"
#include "../../log/log.h"
#include "../error/error.h"
#include "tokenizer.h"

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

static void skip_whitespace (lang_tokenizer_state * state, range_const_unsigned_char * mem)
{
    while (mem->begin < mem->end && isspace (*mem->begin))
    {
	update_position (&state->input_position, *mem->begin);
	mem->begin++;
    }
}

bool lang_token_scan (lang_tokenizer_state * state, range_const_unsigned_char * text)
{
    skip_whitespace (state, text);

    bool escape = false;
    bool quote = false;

    char c;

    for (; !range_is_empty(*text) && (c = *text->begin); text->begin++)
    {
	if (escape)
	{
	    escape = false;
	}
	else if (c == '"')
	{
	    quote = !quote;
	}
	else if (c == '\\')
	{
	    escape = true;
	}
	else if (!quote)
	{
	    if (isspace (c))
	    {
		break;
	    }

	    if (c == '(' || c == ')')
	    {
		break;
	    }

	    update_position (&state->input_position, c);
	}
    }
}

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

    return paren_terminate;
}

static bool skip_source_whitespace (bool * error, lang_tokenizer_state * state, convert_source * source)
{
    window_unsigned_char * buffer = source->contents;
    
    while (true)
    {
	skip_mem_whitespace (state, &buffer->region.const_cast);
	if (range_is_empty (buffer->region))
	{
	    if (!convert_fill_alloc(error, source))
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

static void apply_token_mem (range_const_char * result, lang_tokenizer_state * state, range_const_char * mem, size_t size)
{
    result->begin = mem->begin;
    result->end = mem->begin + size;

    while (mem->begin < result->end)
    {
	update_position (&state->input_position, *mem->begin);
	mem->begin++;
    }

    assert (!range_is_empty (*result));
}

bool lang_tokenizer_read_mem (bool * error, range_const_char * result, lang_tokenizer_state * state, range_const_unsigned_char * mem)
{
    skip_mem_whitespace (state, mem);
    size_t size = 0;
    bool quoted = false;
    
    if (!get_token_size (&size, &quoted, mem))
    {
	lang_log_fatal (state->token_position, "Failed to get token size");
    }

    if (quoted)
    {
	lang_log_fatal (state->token_position, "Unterminated quoted string");
    }

    apply_token_mem (result, state, &mem->char_cast.const_cast, size);

    return !*error;
    
fail:
    *error = true;
    return false;
}
bool lang_tokenizer_read_source (bool * error, range_const_char * result, lang_tokenizer_state * state, convert_source * source)
{
    window_unsigned_char * buffer = source->contents;
    
    window_alloc (*buffer, 1024);

    if (!skip_source_whitespace (error, state, source))
    {
	return false;
    }
    
    size_t size = 0;
    bool quoted = false;

    state->token_position = state->input_position;
    
    while (!get_token_size(&size, &quoted, &buffer->region.const_cast) || size == (size_t) range_count(buffer->region))
    {
	if (!convert_grow (error, source, 64))
	{
	    break;
	}
    }

    if (*error)
    {
	return false;
    }
    
    apply_token_mem (result, state, &buffer->region.char_cast.const_cast, size);
    
    return !*error;
}
