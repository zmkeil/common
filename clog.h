
/***********************************************
  File name		: clog.h
  Create date	: 2016-01-06 19:27
  Modified date : 2016-01-06 19:28
  Author		: zmkeil, alibaba.inc
  Express : 
  
 **********************************************/

namespace common {

extern const char* CLOG_TIPS[10];

#define CALERT 0
#define CWARN 1

#define CLOG(_level_, _fmt_, args...)                    \
    printf("[%s] "_fmt_, CLOG_TIPS[_level_], ##args)

}
