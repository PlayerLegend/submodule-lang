#ifndef FLAT_INCLUDES
#include <stddef.h>
#include <stdbool.h>
#define FLAT_INCLUDES
#include "../../../range/def.h"
#include "../../../window/def.h"
#include "../../../convert/source.h"
#include "../../error/error.h"
#include "../../../immutable/immutable.h"
#include "../../tree/tree.h"
#include "../define/define.h"
#include "../scope/scope.h"
#include "../file/file.h"
#endif

typedef enum {
    FRAME_NOT_SET,
    FRAME_FILE,
    FRAME_SCOPE,
    FRAME_ITER,
}
    pp_stack_frame_type;

typedef struct {
    pp_stack_frame_type type;
    union {
	pp_scope scope;
	pp_file file;
	lang_tree_node ** iter;
    };
}
    pp_stack_frame;

range_typedef(pp_stack_frame*, pp_stack_frame_p);
window_typedef(pp_stack_frame*, pp_stack_frame_p);

void pp_stack_pwd (window_char * result, const range_const_pp_stack_frame_p * stack);

void pp_stack_push_file (immutable_namespace * namespace, window_pp_stack_frame_p * stack, immutable_text path);
