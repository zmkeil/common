#include "common.h"
#include "string_printf.h"

namespace common {

int string_appendf_impl(std::string& output, const char* format, va_list args)
{
    const int write_point = output.size();
    int remain = output.capacity() - write_point;

    output.resize(output.capacity());
    int bytes_used = vsnprintf(&output[write_point], remain, format, args);
    if (bytes_used < 0) {
        return -1;
    } else if (bytes_used < remain) {
        output.resize(write_point + bytes_used);
    } else {
        // maybe bytes_used == remain && just right
        // we take it as error simplicity
        return -1;
    }
    return 0;
}

int string_appendf(std::string* output, const char* format, ...)
{
    va_list args;
    va_start(args, format);
    int rc = string_vappendf(output, format, args);
    va_end(args);
    
    return rc;
}

int string_vappendf(std::string* output, const char* format, va_list args)
{
    if (!output->capacity()) {
        output->reserve(STRING_APPEND_MIN_SIZE);
    }
    const size_t old_size = output->size();
    int rc = string_appendf_impl(*output, format, args);
    if (rc != 0) {
        output->resize(old_size);
    }
    return rc;
}

int string_appendn(std::string* output, const char* str, size_t len)
{
    const int write_point = output->size();
    int remain = output->capacity() - write_point;
    
    len = std::min(remain, (int)len);
    output->resize(write_point + len);
    memcpy(&(*output)[write_point], str, len);
    return 0;
}

}
