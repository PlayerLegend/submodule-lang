#include "tree.h"

#include <assert.h>
#include <stdio.h>
#include <fcntl.h>

#include "../tokenizer/tokenizer.h"


#include "../../window/alloc.h"

#include "../../convert/fd/source.h"

#include "../../log/log.h"

void lang_tree_build_start (lang_tree_build_state * target)
{
    target->result = NULL;
    window_rewrite (target->stack);
    *window_push (target->stack) = &target->result;
}

bool lang_tree_build_update (lang_tree_build_state * target, const lang_token_position * token_position, const range_const_char * token_text)
{
    target->last_position = *token_position;

    if (range_is_empty (*token_text))
    {
	lang_log_fatal (*token_position, "Empty token");
    }
    
    if (range_count(*token_text) == 1 && *token_text->begin == ')')
    {
	assert (!range_is_empty (target->stack.region));
	target->stack.region.end--;

	if (range_is_empty (target->stack.region))
	{
	    lang_log_fatal (target->last_position, "Extra closing paren");
	}
	else
	{
	    return true;
	}
    }
    else
    {
	lang_tree_node * new_node = calloc (1, sizeof(*new_node));
	
	*(target->stack.region.end[-1]) = new_node;
	target->stack.region.end[-1] = &new_node->peer;
	
	if (range_count(*token_text) == 1 && *token_text->begin == '(')
	{
	    *window_push (target->stack) = &new_node->child;
	}
	else
	{
	    new_node->is_text = true;
	    new_node->ref = string_include_range(target->table, token_text);
	}

	new_node->source_position = *token_position;
	new_node->table = target->table;

	return true;
    }

    return true;
    
fail:

    return false;
}

void lang_tree_free (lang_tree_node * root)
{
    if (!root)
    {
	return;
    }
    
    window_lang_tree_node_p stack = {0};
    lang_tree_node tmp;    

    *window_push (stack) = root;

    while (!range_is_empty (stack.region))
    {
	tmp = *stack.region.end[-1];
	free (stack.region.end[-1]);
	if (tmp.peer)
	{
	    stack.region.end[-1] = tmp.peer;
	}
	else
	{
	    stack.region.end--;
	}

	if (!tmp.is_text && tmp.child)
	{
	    *window_push (stack) = tmp.child;
	}
    }

    free (stack.alloc.begin);
}

bool lang_tree_build_finish (lang_tree_build_state * target)
{
    if (range_count (target->stack.region) != 1)
    {
	lang_log_fatal(target->last_position, "Expected closing paren");
    }

    lang_tree_build_clear (target);

    return true;
    
fail:
    lang_tree_free (target->result);
    target->result = NULL;
    lang_tree_build_clear (target);
    return false;
}

void lang_tree_build_clear (lang_tree_build_state * target)
{
    window_clear(target->stack);
}

static void lang_node_print (int depth, lang_tree_node * node)
{
    while (depth > 0)
    {
	putc ('\t', stdout);
	depth--;
    }
    
    if (!node)
    {
	log_normal (")");
    }
    else if (node->is_text)
    {
	log_normal ("[%s]", node->ref->query.key.string);
    }
    else
    {
	log_normal ("(");
    }
}

void lang_tree_print (lang_tree_node * root)
{
    window_lang_tree_node_p stack = {0};
    lang_tree_node * node;

    *window_push (stack) = root;

    log_normal ("(");

    while (!range_is_empty (stack.region))
    {
	node = stack.region.end[-1];
        
	if (!node)
	{
	    stack.region.end--;

	    lang_node_print (range_count(stack.region), node);

	    continue;
	}
	
	lang_node_print (range_count(stack.region), node);

	stack.region.end[-1] = node->peer;

	if (!node->is_text)
	{
	    *window_push (stack) = node->child;
	}
    }

    free (stack.alloc.begin);
}

lang_tree_node * lang_tree_copy (const lang_tree_node * root)
{
    if (!root)
    {
	return NULL;
    }
    
    lang_tree_node * copy_root = malloc (sizeof(*copy_root));

    *copy_root = *root;
    
    window_lang_tree_node_p stack = {0};
    lang_tree_node * node;
    lang_tree_node * copy;

    *window_push (stack) = copy_root;

    while (!range_is_empty (stack.region))
    {
	node = stack.region.end[-1];

	if (!node)
	{
	    stack.region.end--;
	    continue;
	}

	if (node->peer)
	{
	    copy = malloc (sizeof (*copy));
	    *copy = *node->peer;
	    node->peer = copy;
	}

	stack.region.end[-1] = node->peer;

	if (!node->is_text && node->child)
	{
	    copy = malloc (sizeof (*copy));
	    *copy = *node->child;
	    node->child = copy;

	    *window_push (stack) = copy;
	}
    }

    free (stack.alloc.begin);

    return copy_root;
}

bool lang_tree_load_source (lang_tree_node ** result, string_table * table, convert_source * source)
{
    lang_tokenizer_state tokenizer_state = { .input_position.line = 1 };

    lang_tree_build_state build_state = { .table = table };
    
    lang_tree_build_start(&build_state);

    status status;

    while ( STATUS_UPDATE == (status = lang_tokenizer_read_source(&tokenizer_state, source)) )
    {
	if (!lang_tree_build_update(&build_state, &tokenizer_state.token_position, &tokenizer_state.text))
	{
	    lang_log_fatal(tokenizer_state.token_position, "Tree error");
	}
    }
    
    if (status == STATUS_ERROR)
    {
	lang_log_fatal(tokenizer_state.token_position, "Tokenizer error");
    }

    lang_tree_build_finish(&build_state);

    *result = build_state.result;

    return true;

fail:

    lang_tree_build_finish(&build_state);

    lang_tree_free (build_state.result);
    
    return false;
}

bool lang_tree_load_path (lang_tree_node ** result, string_table * table, const char * path)
{
    window_unsigned_char contents = {0};
    fd_source fd_source = fd_source_init (open (path, O_RDONLY), &contents);

    if (fd_source.fd < 0)
    {
	perror(path);
	log_fatal("Could not open a tree file at path %s");
    }

    status status = lang_tree_load_source (result, table, &fd_source.source);

    convert_source_clear (&fd_source.source);

    window_clear (contents);

    return status;

fail:
    return false;
}

bool lang_tree_load_mem (lang_tree_node ** result, string_table * table, range_const_unsigned_char * mem)
{
    lang_tokenizer_state tokenizer_state = { .input_position.line = 1 };

    lang_tree_build_state build_state = { .table = table };
    
    lang_tree_build_start(&build_state);

    status status;
    
    while ( STATUS_UPDATE == (status = lang_tokenizer_read_mem(&tokenizer_state, &mem->char_cast.const_cast)) )
    {
	if (!lang_tree_build_update(&build_state, &tokenizer_state.token_position, &tokenizer_state.text))
	{
	    lang_log_fatal(tokenizer_state.token_position, "Tree error");
	}
    }

    if (status == STATUS_ERROR)
    {
	lang_log_fatal(tokenizer_state.token_position, "Tokenizer error");
    }
    
    lang_tree_build_finish(&build_state);

    return true;

fail:
    
    lang_tree_build_finish(&build_state);
    lang_tree_free (build_state.result);

    return false;
}

bool lang_tree_get_arg_by_ref (lang_tree_node ** result, lang_tree_node * node, string_pair * match)
{
    if (!node)
    {
	return false;
    }

    lang_tree_node * child;
    
    do {
	if (!node->is_text && node->child)
	{
	    child = node->child;

	    if (child->ref == match)
	    {
		assert (child->is_text);
		*result = child->peer;
		return true;
	    }
	}
	
	node = node->peer;
    }
    while (node);

    return false;
}
