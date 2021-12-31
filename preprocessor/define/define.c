#ifndef FLAT_INCLUDES
#include <stddef.h>
#include <stdbool.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#define FLAT_INCLUDES
#include "../../../range/def.h"
#include "../../../range/alloc.h"
#include "../../../window/def.h"
#include "../../../window/alloc.h"
#include "../../../convert/source.h"
#include "../../../log/log.h"
#include "../../error/error.h"
#include "../../../immutable/immutable.h"
#include "../../tree/tree.h"
#include "../../tree/stack.h"
#include "define.h"
#endif

void lang_define_arg_clear (lang_define_arg * target)
{
    lang_tree_free (target->default_value);
    range_clear (target->occurances);
}

void lang_define_clear (lang_define * target)
{
    lang_define_arg * i;

    for_range (i, target->args)
    {
	lang_define_arg_clear(i);
    }

    range_clear (target->args);

    lang_tree_free (target->root);
}

void lang_define_arg_list_occurances (window_lang_tree_node_p * occurances, lang_tree_node * root, immutable_text match)
{
    window_rewrite (*occurances);
    
    window_lang_tree_node_p stack = {0};

    lang_tree_node * node = root;
    
    while (node)
    {
	if (node->is_text)
	{
	    if (node->immutable.text == match.text)
	    {
		*window_push (*occurances) = node;
	    }

	    node = node->peer;
	}
	else if (node->child)
	{
	    lang_tree_stack_push(&stack, node->peer);
	    node = node->child;
	}
	else
	{
	    node = node->peer;
	}

	if (!node)
	{
	    node = lang_tree_stack_pop(&stack);
	}
    }
    
    window_clear (stack);
}


void lang_define_arg_init (lang_define_arg * target, window_lang_tree_node_p * buffer, lang_tree_node * root, immutable_text match)
{
    target->name = match;
    lang_define_arg_list_occurances (buffer, root, match);
    range_copy (target->occurances, buffer->region);
}


bool lang_define_init (lang_define * target, window_lang_define_arg * arg_buffer, window_lang_tree_node_p * occurance_buffer, lang_tree_node * root)
{
    *target = (lang_define){0};

    lang_tree_node * keyword;

    if (!lang_tree_get_child (&keyword, root))
    {
	return false;
    }

    lang_tree_node * name = keyword->peer;

    if (!lang_tree_get_text (&target->name, name))
    {
	return false;
    }

    lang_tree_node * arglist = name->peer;

    lang_tree_node * arg;

    if (!lang_tree_get_child (&arg, arglist))
    {
	return false;
    }

    target->root = lang_tree_copy(arglist->peer);

    lang_tree_node * arg_key;

    window_rewrite (*arg_buffer);

    lang_define_arg * set_arg;

    while (arg)
    {
	set_arg = window_push (*arg_buffer);

	*set_arg = (lang_define_arg) {0};
	
	if (arg->is_text)
	{
	    lang_define_arg_init (set_arg, occurance_buffer, target->root, arg->immutable);
	}
	else
	{
	    arg_key = arg->child;

	    if (!arg_key->is_text)
	    {
		lang_log_fatal (arg->source_position, "Invalid key-value argument syntax");
	    }

	    lang_define_arg_init (set_arg, occurance_buffer, target->root, arg_key->immutable);
	    
	    set_arg->default_value = lang_tree_copy (arg_key->peer);
	}
	
	arg = arg->peer;
    }

    range_copy (target->args, arg_buffer->region);
    
    return true;

fail:
    return false;
}


void lang_define_arg_occurance_set (lang_tree_node * occurance, const lang_tree_node * node)
{
    lang_tree_node * child;

    if (lang_tree_get_child(&child, occurance))
    {
	lang_tree_free (child);
    }

    occurance->is_text = node->is_text;

    if (node->is_text)
    {
	occurance->immutable = node->immutable;
    }
    else
    {
	occurance->child = lang_tree_copy (node->child);
    }
}

void lang_define_arg_set (lang_define_arg * arg, const lang_tree_node * node)
{
    lang_tree_node ** occurance;

    for_range (occurance, arg->occurances)
    {
	lang_define_arg_occurance_set (*occurance, node);
    }

    arg->is_set = true;
}

bool lang_define_arg_is_set (lang_define_arg * arg)
{
    if (arg->is_set)
    {
	return true;
    }
    
    if (arg->default_value)
    {
	lang_define_arg_set (arg, arg->default_value);
	return true;
    }
    else
    {
	return false;
    }
}

lang_tree_node * lang_define_invoke (bool * error, lang_define * define, const lang_tree_node * arg)
{
    lang_define_arg * define_arg;

    const lang_tree_node * arg_root = arg;

    for_range (define_arg, define->args)
    {
	define_arg->is_set = false;
    }

    define_arg = define->args.begin;
    
    while (arg)
    {
	if (define_arg >= define->args.end)
	{
	    lang_log_fatal(arg->source_position, "Too many args provided to macro invocation, should be %zu", range_count(define->args));
	}

	lang_define_arg_set(define_arg, arg);

	define_arg++;

	arg = arg->peer;
    }

    for_range (define_arg, define->args)
    {
	if (!lang_define_arg_is_set(define_arg))
	{
	    lang_log_fatal(arg_root->source_position, "Not enough arguments in macro invocation");
	}
    }

    return lang_tree_copy(define->root);

fail:
    return NULL;
}
