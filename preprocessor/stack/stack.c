#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>
#include <assert.h>
#include <fcntl.h>
#include <stdio.h>
#define FLAT_INCLUDES
#include "../../../keyargs/keyargs.h"
#include "../../../range/def.h"
#include "../../../range/string.h"
#include "../../../range/path.h"
#include "../../../window/def.h"
#include "../../../window/alloc.h"
#include "../../../window/string.h"
#include "../../../window/path.h"
#include "../../../convert/source.h"
#include "../../../convert/fd/source.h"
#include "../../error/error.h"
#include "../../../immutable/immutable.h"
#include "../../tree/tree.h"
#include "../define/define.h"
#include "../scope/scope.h"
#include "../file/file.h"
#include "stack.h"


void pp_stack_pwd (window_char * result, const range_const_pp_stack_frame_p * stack)
{
    window_strcpy (result, ".");
    
    const pp_stack_frame ** frame;

    for_range (frame, *stack)
    {
	if ((*frame)->type == FRAME_FILE)
	{
	    window_path_cat (result, PATH_SEPARATOR, &(*frame)->file.dirname);
	}
    }
}

static lang_tree_node * load_tree (bool * error, immutable_namespace * namespace, const char * path)
{
    window_unsigned_char contents = {0};
    
    fd_source fd_source = fd_source_init (.fd = open (path, O_RDONLY), .contents = &contents);

    if (fd_source.fd < 0)
    {
	perror (path);
	*error = true;
	return NULL;
    }

    lang_tree_node * retval = lang_tree_load (error, namespace, &fd_source.source);

    convert_source_clear(&fd_source.source);
    
    if (*error)
    {
	assert (!retval);
	return NULL;
    }
    
    window_clear (contents);

    convert_source_clear (&fd_source.source);

    return retval;
}

void pp_stack_push_file (immutable_namespace * namespace, window_pp_stack_frame_p * stack, immutable_text path)
{
    pp_stack_frame * file_frame = *window_push(*stack) = calloc (1, sizeof(pp_stack_frame));

    file_frame->type = FRAME_FILE;

    range_const_char basename;

    range_string_init (&basename, path.text);

    file_frame->file.dirname = basename;

    range_basename (&basename, PATH_SEPARATOR);
    
    range_dirname (&file_frame->file.dirname, PATH_SEPARATOR);

    window_char file_path = {0};

    pp_stack_pwd (&file_path, &stack->region.const_cast);

    window_path_cat (&file_path, PATH_SEPARATOR, &basename);

    bool error = false;

    file_frame->file.root = load_tree(&error, namespace, path.text);
    
    pp_stack_frame * iter_frame = *window_push(*stack) = calloc (1, sizeof(pp_stack_frame));

    iter_frame->type = FRAME_ITER;

    iter_frame->iter = &file_frame->file.root;

    window_clear (file_path);
}
