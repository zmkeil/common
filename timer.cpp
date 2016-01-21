#include <time.h>
#include <string.h>
#include "string_printf.h"

namespace common {

char* asctime()
{
    time_t timer;
    struct tm *tblock;
	char* res;
    timer = time(NULL);     //这一句也可以改成time(&timer);
    tblock = localtime(&timer);
	res = ::asctime(tblock);
	res[strlen(res) - 1] = ' ';
	return res;
}

bool local_time(std::string* str_time)
{
    int rc = string_appendf(str_time, asctime());
    return (rc == 0) ? true : false;
}

}
