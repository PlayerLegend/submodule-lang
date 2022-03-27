#include "tokenizer.h"

#include <assert.h>
#include <ctype.h>

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

static void skip_whitespace (lang_token_position * position_diff, range_const_char * mem)
{
    while (mem->begin < mem->end && isspace (*mem->begin))
    {
	update_position (position_diff, *mem->begin);
	mem->begin++;
    }
}

static status lang_token_bound (lang_tokenizer_state * state)
{
    lang_token_position position_start = state->input_position;

    skip_whitespace (&position_start, &state->text);
    
    bool escape = false;
    bool quote = false;

    const char * end = state->text.begin;
    char c;

    for (; end < state->text.end && (c = *end); end++)
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
		if (end == state->text.begin)
		{
		    end++;
		}
		
		break;
	    }
	}
    }

    if (escape || quote)
    {
	return STATUS_ERROR;
    }

    status status = state->text.end == end ? STATUS_END : STATUS_UPDATE;
	
    state->text.end = end;

    state->token_position = position_start;
    state->input_position = state->token_position;
	
    const char * update;
	
    for_range (update, state->text)
    {
	update_position (&state->input_position, *update);
    }

    return status;
}

status lang_tokenizer_read_mem (lang_tokenizer_state * state, range_const_char * text)
{
    state->text = *text;

    status status = lang_token_bound(state);

    if (status == STATUS_ERROR)
    {
	return STATUS_ERROR;
    }
    
    text->begin = state->text.end;

    return status;
}

status lang_tokenizer_read_source (lang_tokenizer_state * state, convert_source * source)
{
    status token_status, read_status;
    
retry:
    
    state->text = source->contents->region.char_cast.const_cast;

    token_status = lang_token_bound(state);

    if (token_status != STATUS_UPDATE)
    {
	read_status = convert_grow (source, 64);

	if (read_status == STATUS_UPDATE)
	{
	    goto retry;
	}
	else if (read_status == STATUS_ERROR)
	{
	    token_status = STATUS_ERROR;
	}
    }
    
    source->contents->region.char_cast.const_cast.begin = state->text.end;

    return token_status;
}
