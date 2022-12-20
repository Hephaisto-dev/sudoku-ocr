#include "error.h"


static int last_err;
static const char* last_msg;
static va_list last_args;


void err_set_last(int err, const char* msg, ...) {
    last_err = err;
    last_msg = msg;
    va_end(last_args);
    va_start(last_args, msg);
}

int err_get_last() {
    return last_err;
}
