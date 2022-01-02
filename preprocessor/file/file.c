#include <stddef.h>
#include <stdbool.h>
#include <assert.h>
#define FLAT_INCLUDES
#include "../../../range/def.h"
#include "../../../range/path.h"
#include "../../../range/string.h"
#include "../../../window/def.h"
#include "../../../window/path.h"
#include "../../../window/string.h"
#include "../../../convert/source.h"
#include "../../error/error.h"
#include "../../../immutable/immutable.h"
#include "../../tree/tree.h"
#include "../../transform/transform.h"
#include "file.h"
#include "../keywords/keywords.h"
#include "../global/global.h"

void transform_file_pwd (window_char * result, const range_const_lang_transform_p * stack)
{
    window_strcpy (result, ".");
    
    const lang_transform ** frame;

    for_range (frame, *stack)
    {
	if ((*frame)->iter == transform_file_iter)
	{
	    window_path_cat (result, PATH_SEPARATOR, &lang_transform_arg_const(*frame, transform_file)->dirname);
	}
    }
}

bool transform_file_new (bool * error, lang_tree_node ** result, lang_transform_state * state, const lang_tree_node * root)
{
    if (!root->is_text)
    {
	return false;
    }
    
    lang_transform * top = lang_transform_alloc(&state->stack, sizeof(transform_file));

    top->iter = transform_file_iter;
    top->clear = transform_file_clear;
    top->result = result;

    transform_file * file = lang_transform_arg(top, transform_file);

    range_const_char basename;
    range_string_init(&basename, root->immutable.text);
    file->dirname = basename;
    range_basename (&basename, PATH_SEPARATOR);
    range_dirname (&file->dirname, PATH_SEPARATOR);

    window_char full_path = {0};
    transform_file_pwd (&full_path, &state->stack.region.const_cast);
    window_path_cat (&full_path, PATH_SEPARATOR, &basename);

    assert (!*error);
    
    file->root = lang_tree_load_path (error, ((transform_global*)state->global)->namespace, full_path.region.begin);

    return !*error;
}

bool transform_file_iter(lang_transform_state * state, lang_transform * top)
{
    transform_file * file = lang_transform_arg(top, transform_file);

    *top->result = file->root;
    file->root = NULL;

    lang_transform_pop (state);

    return true;
}

void transform_file_free(lang_transform_state * state, lang_transform * target)
{
    lang_tree_free (lang_transform_arg(target, transform_file)->root);
}
