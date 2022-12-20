#pragma once

#include "common.h"

#include <stdarg.h>
#include <stdio.h>


typedef enum log_level_e {
    DEBUG,
    INFO,
    WARNING,
    ERROR,
    FATAL
} log_level_t;


void log_base(log_level_t level, FILE* out, const char* format, va_list args);


static inline void log_fatal(const char* format, ...) {
    va_list args;
    va_start(args, format);
    log_base(FATAL, stderr, format, args);
    va_end(args);
}

static inline void log_err(const char* format, ...) {
    va_list args;
    va_start(args, format);
    log_base(ERROR, stderr, format, args);
    va_end(args);
}

static inline void log_warn(const char* format, ...) {
    va_list args;
    va_start(args, format);
    log_base(WARNING, stdout, format, args);
    va_end(args);
}

static inline void log_info(const char* format, ...) {
    va_list args;
    va_start(args, format);
    log_base(INFO, stdout, format, args);
    va_end(args);
}

static inline void log_dbg(const char* format, ...) {
    va_list args;
    va_start(args, format);
    log_base(DEBUG, stdout, format, args);
    va_end(args);
}