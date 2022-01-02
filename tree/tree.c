#include <assert.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#define FLAT_INCLUDES
#include "../../keyargs/keyargs.h"
#include "../../range/def.h"
#include "../../window/def.h"
#include "../../window/alloc.h"
#include "../../immutable/immutable.h"
#include "../error/error.h"
#include "../../convert/source.h"
#include "../../convert/fd/source.h"
#include "../tokenizer/tokenizer.h"
#include "tree.h"
#include "../../log/log.h"

void lang_tree_build_start (lang_tree_build_env * env, immutable_namespace * namespace)
{
    *env = (lang_tree_build_env){0};
    env->root = NULL;
    window_rewrite (env->stack);
    *window_push (env->stack) = &env->root;
    env->namespace = namespace;
}

bool lang_tree_build_update (lang_tree_build_env * env, const lang_token_position * token_position, const range_const_char * token)
{
    env->last_position = *token_position;

    if (range_is_empty (*token))
    {
	lang_log_fatal (*token_position, "Empty token");
    }
    
    if (range_count(*token) == 1 && *token->begin == ')')
    {
	assert (!range_is_empty (env->stack.region));
	env->stack.region.end--;

	if (range_is_empty (env->stack.region))
	{
	    lang_log_fatal (env->last_position, "Extra closing paren");
	}
	else
	{
	    return true;
	}
    }
    else
    {
	lang_tree_node * new_node = calloc (1, sizeof(*new_node));
	
	*(env->stack.region.end[-1]) = new_node;
	env->stack.region.end[-1] = &new_node->peer;
	
	if (range_count(*token) == 1 && *token->begin == '(')
	{
	    *window_push (env->stack) = &new_node->child;
	}
	else
	{
	    new_node->is_text = true;

	    range_const_char mod_token = *token;

	    if (*mod_token.begin == '"')
	    {
		if (range_count (*token) < 2 || mod_token.end[-1] != '"')
		{
		    lang_log_fatal (*token_position, "Unterminated quote");
		}

		new_node->is_quoted = true;

		mod_token.begin++;
		mod_token.end--;
	    }
	    
	    new_node->immutable = immutable_string_range(env->namespace, &mod_token);
	}

	new_node->source_position = *token_position;

	return true;
    }

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

lang_tree_node * lang_tree_build_finish (bool * error, lang_tree_build_env * env)
{
    if (range_count (env->stack.region) != 1)
    {
	lang_tree_free (env->root);
	lang_log_fatal (env->last_position, "Expected closing paren");
    }

    lang_tree_node * root = env->root;
    
    lang_tree_build_clear (env);
    
    return root;

fail:

    lang_tree_build_clear (env);
    *error = true;
    return NULL;
}

void lang_tree_build_clear (lang_tree_build_env * env)
{
    window_clear(env->stack);
    *env = (lang_tree_build_env){0};
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
	log_normal ("%s [%s]", node->is_quoted ? "quote" : "lit", node->immutable.text);
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

lang_tree_node * lang_tree_load_source (bool * error, immutable_namespace * namespace, convert_source * source)
{
    range_const_char token;
    
    lang_tokenizer_state tokenizer_state = { .source = source };

    lang_tree_build_env build_env;

    lang_tree_build_start(&build_env, namespace);

    tokenizer_state.input_position.line = 1;

    while (lang_tokenizer_read (error, &token, &tokenizer_state))
    {
	if (!lang_tree_build_update(&build_env, &tokenizer_state.token_position, &token))
	{
	    *error = true;
	    return NULL;
	}
    }

    if (*error)
    {
	lang_tree_build_clear(&build_env);
	return NULL;
    }

    return lang_tree_build_finish(error, &build_env);
}

lang_tree_node * lang_tree_load_path (bool * error, immutable_namespace * namespace, const char * path)
{
    window_unsigned_char contents = {0};
    fd_source fd_source = fd_source_init (.fd = open (path, O_RDONLY), .contents = &contents);
    if (fd_source.fd < 0)
    {
	perror (path);
	*error = true;
	return NULL;
    }
    lang_tree_node * retval = lang_tree_load_source (error, namespace, &fd_source.source);

    convert_source_clear (&fd_source.source);

    window_clear (contents);

    return retval;
}
