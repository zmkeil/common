
/***********************************************
  File name		: iobuf.cpp
  Create date	: 2016-01-21 20:05
  Modified date : 2016-01-21 20:06
  Author		: zmkeil, alibaba.inc
  Express : 
  
 **********************************************/
#include "iobuf.h"

namespace common {
bool IOBuf::init_pool() {return true;}

bool IOBuf::alloc_next_block() {return true;}
bool IOBuf::is_current_block_buf_enough(size_t size, IOBufAllocType type) {(void) size; (void) type; return true;}
size_t IOBuf::current_block_remain_buf_size() {return 0;}
size_t IOBuf::current_block_alloc_buf_size() {return 0;}
char* IOBuf::alloc_from_current_block(size_t n) {(void) n; return NULL;}
bool IOBuf::reclaim_to_current_block(size_t n) {(void) n; return true;}

void IOBuf::move_read_point_to_next_block() {}
size_t IOBuf::current_block_remain_data_size() {return 0;}
size_t IOBuf::current_block_consume_data_size() {return 0;}
}
