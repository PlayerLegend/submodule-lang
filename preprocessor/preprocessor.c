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

inline static bool get_text (bool * error, immutable_text * text, lang_tree_node * node) // potential to recursively evaluate macros here
{
    if (!node || !node->is_text)
    {
	return false;
    }

    *text = node->immutable;

    return true;
}

inline static bool is_text (bool * error, lang_tree_node * node)
{
    immutable_text text;

    return get_text (error, &text, node);
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

    bool error = false;

    while (arg)
    {
	set_arg = window_push (buffers->arg);

	*set_arg = (lang_define_arg) {0};
	
	if (get_text (&error, &arg_name, arg))
	{
	    setup_define_arg (set_arg, &buffers->immutable, target->root, arg_name);
	}
	else if(!error)
	{
	    arg_key = arg->child;

	    if (!get_text (&error, &arg_name, arg_key))
	    {
		lang_log_fatal (arg->source_position, "Invalid key-value argument syntax");
	    }

	    setup_define_arg (set_arg, &buffers->immutable, target->root, arg_name);
	    
	    arg_value = arg_key->peer;

	    set_arg->default_value = lang_tree_copy (arg_value);
	}
	else
	{
	    return false;
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
    lang_tree_node ** target;
}
    lang_namespace;

range_typedef (lang_namespace, lang_namespace);
window_typedef (lang_namespace, lang_namespace);

static void lang_namespace_clear (lang_namespace * target)
{
    window_clear_type (target->defines, lang_define);
}

typedef struct {
    range_const_char path;
    range_const_char dirname;

    lang_tree_node ** insertion_point;

    lang_tree_node * root;
}
    lang_file;

range_typedef(lang_file,lang_file);
window_typedef(lang_file,lang_file);

typedef struct {
    window_lang_file files;
    window_lang_namespace namespaces;
    immutable_namespace * immutable_namespace;
}
    lang_state;

static void lang_state_clear (lang_state * target)
{
    window_clear (target->files);
    window_clear_type (target->namespaces, lang_namespace);
}

static void delete_node (lang_tree_node ** node)
{
    if (!*node)
    {
	return;
    }
    
    lang_tree_node * to_free = *node;

    *node = to_free->peer;

    to_free->peer = NULL;

    lang_tree_free(to_free);
}

static bool lang_add_define_to_namespace (lang_namespace * namespace, memory_buffers * buffers, lang_tree_node * define_root)
{
    return setup_define (window_push (namespace->defines), buffers, define_root);
}

static bool lang_add_define_to_state (lang_state * state, memory_buffers * buffers, lang_tree_node * define_root)
{
    assert (!range_is_empty (state->namespaces.region));

    return lang_add_define_to_namespace (state->namespaces.region.end - 1, buffers, define_root);
}



static void lang_state_resolve_path (window_char * path, lang_state * state)
{
    window_strcpy (path, ".");

    if (range_is_empty (state->files.region))
    {
	return;
    }
    
    lang_file * i;

    range_lang_file parents = state->files.region;

    parents.end--;

    for_range (i, parents)
    {
	window_path_cat(path, PATH_SEPARATOR, &i->dirname);
    }

    window_path_cat (path, PATH_SEPARATOR, &state->files.region.end[-1].path);
}

static lang_tree_node * lang_state_load_path (lang_state * state, memory_buffers * buffers)
{
    lang_state_resolve_path (&buffers->path, state);

    log_debug ("loading path " RANGE_FORMSPEC, RANGE_FORMSPEC_ARG(buffers->path.region));
    
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


static bool lang_state_start_file (lang_state * state, memory_buffers * buffers, lang_tree_node ** insertion_point)
{
    lang_tree_node * include_root = *insertion_point;

    assert (include_root);
    assert (!include_root->is_text);

    lang_tree_node * include_keyword = include_root->child;

    bool error = false;
    assert (is_text (&error, include_keyword));

    lang_tree_node * include_path = include_keyword->peer;

    immutable_text include_path_text = {0};

    if (!get_text (&error, &include_path_text, include_path))
    {
	lang_log_fatal (include_root->source_position, "Could not get include path in include");
    }

    lang_file * new_file = window_push (state->files);

    range_string_init (&new_file->path, include_path_text.text);

    new_file->dirname = new_file->path;
    range_dirname (&new_file->dirname, PATH_SEPARATOR);

    new_file->root = lang_state_load_path (state, buffers);
    
    new_file->insertion_point = insertion_point;

    delete_node (insertion_point);

    return true;

fail:
    return false;
}

static bool lang_state_finish_file (lang_state * state)
{
    assert (!range_is_empty (state->files.region));

    lang_file * finish_file = --state->files.region.end;

    if (!finish_file->root)
    {
	return true;
    }

    lang_tree_node ** file_end = &finish_file->root->peer;

    while (*file_end)
    {
	*file_end = (*file_end)->peer;
    }

    *file_end = *finish_file->insertion_point;

    *finish_file->insertion_point = finish_file->root;

    return true;

}

typedef struct {
    immutable_text define;
    immutable_text include;
}
    lang_keywords;

static bool lang_iterate (lang_state * state, memory_buffers * buffers, const lang_keywords * keywords)
{
    assert (!range_is_empty (state->namespaces.region));
    
    lang_namespace * top_namespace = state->namespaces.region.end - 1;

    lang_tree_node ** target = top_namespace->target;

    lang_tree_node * target_child;

    bool error = false;
	    
    if (is_text (&error, *target) || !(target_child = (*target)->child))
    {
	*target = (*target)->peer;

	return true;
    }

    if (error)
    {
	return false;
    }

    immutable_text target_child_text = {0};

    if (!get_text(&error, &target_child_text, target_child))
    {
	return !error;
    }
    
    if (target_child_text.text == keywords->define.text)
    {
	if (lang_add_define_to_state(state, buffers, *target))
	{
	    delete_node (target);
	    return true;
	}
	else
	{
	    return false;
	}
    }
    else if (target_child->immutable.text == keywords->include.text)
    {
	return lang_state_start_file(state, buffers, target);
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
