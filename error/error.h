#ifndef FLAT_INCLUDES
#define FLAT_INCLUDES
#include "../../log/log.h"
#endif

typedef struct {
    int col;
    int line;
}
    lang_token_position;

#define lang_log_warning(position, fmt, ...)	\
    log_stderr("warning:%d:%d: " fmt, (position).line, (position).col, ##__VA_ARGS__)

#define lang_log_fatal(position, fmt, ...)				\
    { log_stderr("fatal:%d:%d: " fmt, (position).line, (position).col, ##__VA_ARGS__); goto fail; }
