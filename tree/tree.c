#include <assert.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#define FLAT_INCLUDES
#include "../../range/def.h"
#include "../../window/def.h"
#include "../../window/alloc.h"
#include "../../immutable/immutable.h"
#include "../error/error.h"
#include "tree.h"
#include "../../log/log.h"

range_typedef (lang_tree_node*, lang_tree_node_p);
window_typedef (lang_tree_node*, lang_tree_node_p);

void lang_tree_build_start (lang_tree_build_env * env)
{
    env->root = NULL;
    window_rewrite (env->stack);
    *window_push (env->stack) = &env->root;
}

bool lang_tree_build_update (lang_tree_build_env * env, lang_token_position * token_position, immutable_text token_text)
{
    env->last_position = *token_position;
    
    if (*token_text.text == ')')
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
	
	if (*token_text.text == '(')
	{
	    *window_push (env->stack) = &new_node->child;
	}
	else
	{
	    new_node->is_text = true;
	    new_node->immutable.text = token_text.text;
	}

	new_node->source_position = *token_position;

	return true;
    }

fail:
    return false;
}

void lang_tree_free (lang_tree_node * root)
{
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

lang_tree_node * lang_tree_build_finish (lang_tree_build_env * env)
{
    if (range_count (env->stack.region) != 1)
    {
	lang_tree_free (env->root);
	lang_log_fatal (env->last_position, "Expected closing paren");
    }

    lang_tree_node * root = env->root;
    
    env->root = NULL;

    return root;

fail:
    return false;
}

void lang_tree_build_clear (lang_tree_build_env * env)
{
    free (env->stack.region.begin);
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
	log_normal ("[%s]", node->immutable.text);
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

lang_tree_node * lang_tree_copy (lang_tree_node * root)
{
    assert (root);
    
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
