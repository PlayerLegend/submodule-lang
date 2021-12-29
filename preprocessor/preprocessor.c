#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <fcntl.h>
#include <unistd.h>
#define FLAT_INCLUDES
#include "../../keyargs/keyargs.h"
#include "../../range/def.h"
#include "../../range/string.h"
#include "../../range/alloc.h"
#include "../../window/def.h"
#include "../../window/string.h"
#include "../../range/path.h"
#include "../../window/alloc.h"
#include "../../window/path.h"
#include "../../convert/source.h"
#include "../../convert/fd/source.h"
#include "../../immutable/immutable.h"
#include "../../log/log.h"
#include "../error/error.h"
#include "../tree/tree.h"
#include "../tree/stack.h"
#include "../tokenizer/tokenizer.h"
#include "preprocessor.h"

#define PATH_SEPARATOR '/'

typedef struct {
    immutable_text name;
    lang_tree_node * default_value;
    range_lang_tree_node_p occurances;
}
    lang_define_arg;

static void lang_define_arg_clear (lang_define_arg * target)
{
    lang_tree_free (target->default_value);
    lang_tree_node ** i;

    for_range (i, target->occurances)
    {
	(*i)->is_text = true; // so that the tree free function doesn't try to descend into the child here, as it is aliased from elsewhere if it's a tree.
	(*i)->immutable.text = NULL;
    }
}

static void find_define_arg_occurances (window_lang_tree_node_p * occurances, lang_tree_node * root, immutable_text match)
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
	}
	else if (node->peer)
	{
	    lang_tree_stack_push(&stack, node->child);
	    node = node->peer;
	}
	else if (node->child)
	{
	    node = node->child;
	}
	else
	{
	    node = lang_tree_stack_pop(&stack);
	}
    }
    
    window_clear (stack);
}

range_typedef (lang_define_arg, lang_define_arg);
window_typedef (lang_define_arg, lang_define_arg);

typedef struct {
    window_lang_tree_node_p immutable;
    window_lang_define_arg arg;
    window_char path;
    window_unsigned_char file;
}
    memory_buffers;

static void memory_buffers_clear (memory_buffers * target)
{
    window_clear (target->immutable);
    window_clear (target->arg);
    window_clear (target->path);
}

static void setup_define_arg (lang_define_arg * target, window_lang_tree_node_p * buffer, lang_tree_node * root, immutable_text match)
{
    target->name = match;
    find_define_arg_occurances(buffer, root, match);
    range_copy (target->occurances, buffer->region);
}

typedef struct {
    lang_tree_node * root;
    range_lang_define_arg args;
}
    lang_define;

static void lang_define_clear (lang_define * target)
{
    lang_define_arg * i;

    for_range (i, target->args)
    {
	lang_define_arg_clear(i);
    }

    range_clear (target->args);

    lang_tree_free (target->root);
}

inline static bool get_text (immutable_text * text, lang_tree_node * node) // potential to recursively evaluate macros here
{
    if (!node || !node->is_text)
    {
	return false;
    }

    *text = node->immutable;

    return true;
}

static bool setup_define (lang_define * target, memory_buffers * buffers, lang_tree_node * define_root)
{
    *target = (lang_define){0};
    
    assert (define_root);
    assert (!define_root->is_text);

    lang_tree_node * keyword = define_root->child;

    assert (keyword && keyword->is_text);

    lang_tree_node * arglist = keyword->peer;

    if (arglist->is_text)
    {
	lang_log_fatal(arglist->source_position, "Macro arglist should be a list");
    }

    lang_tree_node * body_root = arglist->peer;

    target->root = lang_tree_copy(body_root);

    lang_tree_node * arg = arglist->child;

    lang_tree_node * arg_key;

    immutable_text arg_name;
    
    lang_tree_node * arg_value;

    window_rewrite (buffers->arg);

    lang_define_arg * set_arg;

    while (arg)
    {
	set_arg = window_push (buffers->arg);

	*set_arg = (lang_define_arg) {0};
	
	if (get_text (&arg_name, arg))
	{
	    setup_define_arg (set_arg, &buffers->immutable, target->root, arg_name);
	}
	else
	{
	    arg_key = arg->child;

	    if (!get_text (&arg_name, arg_key))
	    {
		lang_log_fatal (arg->source_position, "Invalid key-value argument syntax");
	    }

	    setup_define_arg (set_arg, &buffers->immutable, target->root, arg_name);
	    
	    arg_value = arg_key->peer;

	    set_arg->default_value = lang_tree_copy (arg_value);
	}
	
	arg = arg->peer;
    }

    range_copy (target->args, buffers->arg.region);
    
    return true;

fail:
    return false;
}

range_typedef (lang_define, lang_define);
window_typedef (lang_define, lang_define);

typedef struct {
    window_lang_define defines;
    size_t tree_depth;
}
    lang_namespace;

static void lang_namespace_clear (lang_namespace * target)
{
    lang_define * i;

    for_range (i, target->defines.region)
    {
	lang_define_clear(i);
    }

    window_clear (target->defines);
}

range_typedef (lang_namespace, lang_namespace);
window_typedef (lang_namespace, lang_namespace);

typedef struct {
    range_const_char path;
    size_t tree_depth;
}
    lang_directory;

range_typedef(lang_directory, lang_directory);
window_typedef(lang_directory, lang_directory);

typedef struct {
   
    struct {
	window_lang_directory directory;
	window_lang_tree_node_pp iter;
	window_lang_namespace namespace;
    }
	stack;
    
    immutable_namespace * immutable_namespace;
}
    lang_state;

static bool lang_apply_define (lang_state * state, memory_buffers * buffers, lang_tree_node ** define_root)
{
    size_t tree_depth = range_count (state->stack.iter.region);

    if (range_is_empty (state->stack.namespace.region) || tree_depth < state->stack.namespace.region.end[-1].tree_depth)
    {
	*window_push (state->stack.namespace) = (lang_namespace) { .tree_depth = tree_depth };
    }

    lang_namespace * top_namespace = state->stack.namespace.region.end - 1;

    if (!setup_define (window_push (top_namespace->defines), buffers, *define_root))
    {
	return false;
    }

    lang_tree_node * to_free = *define_root;

    *define_root = to_free->peer;

    lang_tree_free(to_free);

    return true;
}

static void lang_state_clear (lang_state * target)
{
    lang_namespace * i;
	
    for_range (i, target->stack.namespace.region)
    {
	lang_namespace_clear (i);
    }
	
    window_clear (target->stack.namespace);
    window_clear (target->stack.directory);
    window_clear (target->stack.iter);
}

static void lang_state_add_dir (lang_state * state, range_const_char * immutable_path)
{
    assert (range_is_empty (state->stack.directory.region) || state->stack.directory.region.end[-1].tree_depth < (size_t) range_count(state->stack.iter.region));
    
    range_const_char dirname = *immutable_path;

    range_dirname (&dirname, PATH_SEPARATOR);
    
    *window_push (state->stack.directory) = (lang_directory)
    {
	.path = dirname,
	.tree_depth = range_count (state->stack.iter.region)
    };
}

static void lang_state_add_todo (lang_state * state, lang_tree_node ** todo)
{
    *window_push (state->stack.iter) = todo;
}

static lang_tree_node ** lang_state_pop_todo (lang_state * state)
{
    size_t stack_size = range_count (state->stack.iter.region);
    if (!stack_size)
    {
	return NULL;
    }

    state->stack.iter.region.end--;
    stack_size--;
    
    if (!range_is_empty (state->stack.directory.region))
    {
	assert (state->stack.directory.region.end[-1].tree_depth <= stack_size);
	if (state->stack.directory.region.end[-1].tree_depth == stack_size)
	{
	    state->stack.directory.region.end--;
	}
	assert (range_is_empty(state->stack.directory.region) || state->stack.directory.region.end[-1].tree_depth < stack_size);
    }

    if (!range_is_empty (state->stack.namespace.region))
    {
	assert (state->stack.namespace.region.end[-1].tree_depth <= stack_size);
	if (state->stack.namespace.region.end[-1].tree_depth == stack_size)
	{
	    state->stack.namespace.region.end--;
	    lang_namespace_clear(state->stack.namespace.region.end);
	}
	assert (range_is_empty(state->stack.namespace.region) || state->stack.namespace.region.end[-1].tree_depth < stack_size);
    }

    
    return *state->stack.iter.region.end;
}

static void lang_state_resolve_path (window_char * path, lang_state * state, const range_const_char * append)
{
    window_strcpy (path, ".");

    lang_directory * i;

    for_range (i, state->stack.directory.region)
    {
	window_path_cat(path, PATH_SEPARATOR, &i->path);
    }

    window_path_cat (path, PATH_SEPARATOR, append);

    window_path_resolve(path, PATH_SEPARATOR);
}

static lang_tree_node * lang_state_load_path (lang_state * state, memory_buffers * buffers, const range_const_char * path)
{
    lang_state_resolve_path (&buffers->path, state, path);

    window_rewrite (buffers->file);
    
    fd_source fd_source = fd_source_init (.fd = open (buffers->path.region.begin, O_RDONLY), .contents = &buffers->file);

    if (fd_source.fd < 0)
    {
	return NULL;
    }

    lang_tree_node * retval = lang_tree_load(state->immutable_namespace, &fd_source.source);

    close (fd_source.fd);

    return retval;
}

typedef struct {
    immutable_text define;
    immutable_text include;
}
    lang_keywords;

static bool lang_parse_keywords (lang_state * state, lang_tree_node ** target, memory_buffers * buffers, const lang_keywords * keywords)
{
    
    if (!(*target)->is_text && (*target)->child)
    {
	lang_tree_node * target_child = (*target)->child;
	    
	if (target_child->is_text)
	{
	    if (target_child->immutable.text == keywords->define.text)
	    {
		if (!lang_apply_define(state, buffers, target))
		{
		    return false;
		}
	    }
	    else if (target_child->immutable.text == keywords->include.text)
	    {
		    
	    }
	}

	    
    }
}

static lang_tree_node * lang_build (immutable_namespace * namespace, const range_const_char * path)
{
    lang_state state = { .immutable_namespace = namespace };
    memory_buffers buffers = {0};

    lang_tree_node * retval = lang_state_load_path (&state, &buffers, path);

    lang_tree_node ** node = &retval;

    lang_keywords keywords =
	{
	    .define = immutable_string (namespace, "define"),
	    .include = immutable_string (namespace, "include"),
	};

    while (*node)
    {
	if (!(*node)->is_text && (*node)->child)
	{
	    if (!lang_parse_keywords(&state, node, &buffers, &keywords))
	    {
		log_fatal ("Failed to parse a keyword");
	    }
	}
    }

    lang_state_clear (&state);
    memory_buffers_clear(&buffers);

    return retval;
    
fail:
    lang_state_clear (&state);
    memory_buffers_clear(&buffers);
    lang_tree_free (retval);
    return NULL;
}
