#include "log.h"

void log_base(log_level_t level, FILE* out, const char* format, va_list args) {
    switch(level) {
        case DEBUG:
            fwrite("[Debug] ", 1, 8, out);
            break;
        case INFO:
            fwrite("[Info] ", 1, 7, out);
            break;
        case WARNING:
            fwrite("[Warning] ", 1, 10, out);
            break;
        case ERROR:
            fwrite("[Error] ", 1, 8, out);
            break;
        case FATAL:
            fwrite("[Fatal] ", 1, 8, out);
            break;
    }
    vfprintf(out, format, args);
    fputc('\n', out);
    fflush(out);
}
