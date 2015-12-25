#ifndef COMMON_STRING_PRINTF_H
#define COMMON_STRING_PRINTF_H

#include <stdarg.h>
#include <string>

#define STRING_APPEND_MIN_SIZE 256

namespace common {

int string_appendf(std::string* output, const char* format, ...)
    __attribute__ ((format (printf, 2, 3)));

int string_vappendf(std::string* output, const char* format, va_list args);

}

#endif
