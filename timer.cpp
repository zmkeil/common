#include "common.h"
#include "string_printf.h"

namespace common {

bool local_time(std::string* str_time)
{
    time_t timer;
    struct tm *tblock;
    timer = time(NULL);     //这一句也可以改成time(&timer);
    tblock = localtime(&timer);

    int rc = string_appendf(str_time, asctime(tblock));
    return (rc == 0) ? true : false;
}

}
