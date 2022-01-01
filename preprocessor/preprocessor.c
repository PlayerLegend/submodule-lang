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
    lang_tree_node ** in;
    lang_tree_node ** out;
}
    lang_define_arg_occurance;

range_typedef(lang_define_arg_occurance, lang_define_arg_occurance);
window_typedef(lang_define_arg_occurance, lang_define_arg_occurance);

typedef struct {
    immutable_text name;
    lang_tree_node * default_value;
    range_lang_define_arg_occurance occurances;
    bool is_set;
}
    lang_define_arg;


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

range_typedef (lang_define_arg, lang_define_arg);
window_typedef (lang_define_arg, lang_define_arg);

typedef struct {
    window_lang_define_arg_occurance occurance;
    window_lang_define_arg arg;
    window_char path;
    window_unsigned_char file;
}
    memory_buffers;

static void memory_buffers_clear (memory_buffers * target)
{
    window_clear (target->occurance);
    window_clear (target->arg);
    window_clear (target->path);
}



typedef enum {
    FRAME_NOT_SET,
    FRAME_FILE,
    FRAME_SCOPE,
    FRAME_ASSIGN,
}
    stack_frame_type;

typedef struct {
    stack_frame_type type;
    union {
	lang_scope scope;
	lang_file file;
	lang_tree_node ** assign;
    };
}
    stack_frame;

typedef struct {
    memory_buffers buffers;
    lang_keywords keywords;
    immutable_namespace * namespace;
}
    global;

range_typedef(stack_frame*, stack_frame_p);
window_typedef(stack_frame*, stack_frame_p);

static void resolve_path (window_char * path, const range_const_stack_frame_p * stack)
{
    window_strcpy (path, ".");
    
    range_const_stack_frame_p stack_parents = *stack;

    stack_parents.end--;

    const stack_frame ** i;

    const stack_frame * this_file;
    const stack_frame * previous_file;

    for_range (i, stack_parents)
    {
	this_file = *i;
	if (this_file->type == FRAME_FILE)
	{
	    window_path_cat(path, PATH_SEPARATOR, &this_file->file.dirname);
	    previous_file = this_file;
	}
    }

    assert (previous_file);

    if (previous_file)
    {
	window_path_cat(path, PATH_SEPARATOR, &previous_file->file.basename);
    }
}

static bool add_file (window_stack_frame_p * stack, global * global, immutable_text file_name)
{
    *window_push(*stack) = calloc (1, sizeof(stack_frame));
    *window_push(*stack) = calloc (1, sizeof(stack_frame));

    stack_frame * new_file = stack->region.end[-2];
    stack_frame * new_assign = stack->region.end[-1];

    new_assign->type = FRAME_ASSIGN;
    
    new_file->type = FRAME_FILE;

    range_string_init(&new_file->file.basename, file_name.text);

    new_file->file.dirname = new_file->file.basename;

    range_basename (&new_file->file.basename, PATH_SEPARATOR);
    range_dirname (&new_file->file.dirname, PATH_SEPARATOR);

    resolve_path (&global->buffers.path, &stack->region.const_cast);

    fd_source fd_source = fd_source_init (.fd = open (global->buffers.path.region.begin, O_RDONLY), .contents = &global->buffers.file);

    if (fd_source.fd < 0)
    {
	return false;
    }

    bool error = false;
    
    new_file->file.root = lang_tree_load(&error, global->namespace, &fd_source.source);

    close (fd_source.fd);

    if (error)
    {
	return false;
    }

    *new_assign = (stack_frame) { .type = FRAME_ASSIGN, .assign = &new_file->file.root };

    return true;
}

static void add_scope (window_stack_frame_p * stack, lang_tree_node ** child)
{
    *window_push (*stack) = calloc (1, sizeof(stack_frame));
    *window_push (*stack) = calloc (1, sizeof(stack_frame));
    
    stack_frame * new_scope = stack->region.end[-2];
    stack_frame * new_assign = stack->region.end[-1];

    new_assign->type = FRAME_ASSIGN;    
    new_scope->type = FRAME_SCOPE;

    new_assign->assign = child;
}

static bool add_define (range_stack_frame_p * stack, memory_buffers * buffers, lang_tree_node * define_root)
{
    stack_frame ** i_frame;

    for (i_frame = stack->end - 1; i_frame >= stack->begin; i_frame--)
    {
	if ((*i_frame)->type != FRAME_SCOPE)
	{
	    continue;
	}

	if (!setup_define (window_push ((*i_frame)->scope.defines), buffers, define_root))
	{
	    return false;
	}

	return true;
    }

    return false;
}

static lang_define * lookup_define (const range_const_stack_frame_p * stack, immutable_text name)
{
    const stack_frame ** i_frame;
    lang_define * i_define;
    
    for (i_frame = stack->end - 1; i_frame >= stack->begin; i_frame--)
    {
	if ((*i_frame)->type != FRAME_SCOPE)
	{
	    continue;
	}

	for_range (i_define, (*i_frame)->scope.defines.region)
	{
	    if (i_define->name.text == name.text)
	    {
		return i_define;
	    }
	}
    }

    return NULL;
}

static void init_args (lang_define * define)
{
    lang_define_arg * arg;

    for_range (arg, define->args)
    {
	arg->is_set = false;
    }
}

static bool apply_define (bool * error, lang_tree_node ** root, const range_const_stack_frame_p * stack)
{
    lang_tree_node * name_node;

    if (!get_child(&name_node, *root))
    {
	return false;
    }

    immutable_text name;

    if (!get_text (&name, name_node))
    {
	return false;
    }

    lang_define * define = lookup_define (stack, name);

    if (!define)
    {
	return false;
    }
    
    lang_tree_node * arglist = name_node->peer;

    lang_tree_node * arg;

    if (!get_child(&arg, arglist))
    {
	lang_log_fatal ((*root)->source_position, "Macro invocation must have an arglist");
    }

    init_args (define);
    
    while (arg)
    {
	
    }

fail:
    *error = true;
    return false;
}
