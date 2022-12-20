#pragma once

#include "common.h"

#include <stdarg.h>


// List of errors code

#define ERROR_SUCCESS           0
#define ERROR_UNKNOWN           1


const char* ERROR_MESSAGES[] = {
    [ERROR_SUCCESS] = "Not an error",
    [ERROR_UNKNOWN] = "Unknown error (you should don't have this error message unless there is bad code somewhere :D)"
};
const int ERROR_MESSAGES_LEN = sizeof(ERROR_MESSAGES) * sizeof(ERROR_MESSAGES[0]);



void err_set_last(int err, const char* msg, ...);
int err_get_last_code();
const char* err_get_last_msg();
//va_list err_get_last_args();

inline const char* err_get_msg(int err) {
    return ERROR_MESSAGES[(err < 0 || err >= ERROR_MESSAGES_LEN) ? 1 : err];
}
