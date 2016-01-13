
/***********************************************
  File name		: clog.h
  Create date	: 2016-01-06 19:27
  Modified date : 2016-01-11 20:08
  Author		: zmkeil, alibaba.inc
  Express : 
  
 **********************************************/

#include "common_type.h"

namespace common {

extern com_flag_t COMMON_TEST_FLAG;
extern const char* CLOG_TIPS[10];

#define CALERT 0
#define CWARN 1

// for debug, set COMMON_TEST_FLAG = 1
#define CLOG(_level_, _fmt_, args...)                       \
do{                                                         \
    if (COMMON_TEST_FLAG) {                                 \
        printf("[clog - %s] "                                      \
                _fmt_, CLOG_TIPS[_level_], ##args);         \
    }                                                       \
} while(0)

}
