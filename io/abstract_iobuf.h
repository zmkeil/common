#ifndef COMMON_IO_ABSTRACT_IOBUF_H
#define COMMON_IO_ABSTRACT_IOBUF_H

#include <stdio.h>
#include <string>
#include "comlog/info_log_context.h"

namespace common {

enum IOBufAllocType {
	IOBUF_ALLOC_EXACT = 0,
	IOBUF_ALLOC_SIMILAR
};

class AbstractIOBuf 
{
public:
    AbstractIOBuf(size_t block_size) :
			_block_size(block_size),
			_bytes(0),
			_blocks(0),
			_read_block(0),
			_read_point(nullptr),
			_is_read_point_cached(false)
	{
	}

    virtual ~AbstractIOBuf() {}

/* For input */
    // alloc buf from current block(pool) or get a new block if not enough buf in current block, 
	// the "size" must LT _block_size,
	// return the actually alloc size
    int alloc(char** buf, size_t size, IOBufAllocType type)
	{
		if (!_read_point) {
			if (!init_pool()) {
				LOG(ALERT, "iobuf init_pool failed");
				return -1;
			}
		}

		if (size > _block_size) {
			LOG(ALERT, "iobuf alloc size GT block_size");
			return -1;
		}

		if (!is_current_block_buf_enough(size, type)) {
			if (!alloc_next_block()) {
				LOG(ALERT, "get_next_block failed");
				return -1;
			}
		}

		size_t n = std::min(size, current_block_remain_buf_size());
		*buf = alloc_from_current_block(n);
		_bytes += n;
		return (int)n;
	}
	int alloc(char** buf)
	{
		return alloc(buf, _block_size, IOBUF_ALLOC_SIMILAR);
	}

	// reclaim buf back to pool
	// the count must LE current block aclloc size
	// this is OK for ZeroCopyStream
    bool reclaim(size_t count)
	{
		size_t n = current_block_alloc_buf_size();
		if (n < count) {
			LOG(ALERT, "reclaim count GT current block alloc buf size");
			return false;
		}
		reclaim_to_current_block(count);
		_bytes -= count;
		return true;
	}

/* For output */
	// get data from current block
    // return actually read size, maybe 0 if current block has no data more, 
	// in such condition, move _read_point to next block
    int read(const char** data)
	{
		if (!_read_point) {
			LOG(ALERT, "no more data in zero_copy_input_stream");
			return -1;
		}

		size_t remain = current_block_remain_data_size();
		if (remain > 0) {
			// when cutted, we can only read _bytes
			size_t n = std::min(remain, _bytes);
			*data = _read_point;
			_read_point += n;
			_bytes -= n;
			return (int)n;
		}

		// else move to next block, and then just wait for next read
		move_read_point_to_next_block();
		*data = _read_point;
		return 0/*OK, for next read*/;
	}

	// skip the data in one or more block
	// the "count" must LT _byte_count
    size_t skip(size_t count)
	{
		size_t origin_count = count;
		while (count && _bytes) {
			size_t n = std::min(current_block_remain_data_size(), _bytes)/*for cut*/;
			if (n < count) {
				_read_point += n;
				_bytes += n;
				count -= n;
				move_read_point_to_next_block();
				if (!_read_point) {
					break;
				}
			}
			else {
				_read_point += count;
				_bytes -= count;
				// skip over
				count = 0;
			}
		}

		if (count) {
			LOG(ALERT, "skip too much, not so much data");
		}
		return origin_count - count;
	}

	// skip(_byte_count)
    void release_all()
	{
		skip(_bytes);
	}

	// back some data for later read
    void back(size_t count)
	{
		size_t n = current_block_consume_data_size();
		if (n < count) {
			LOG(ALERT, "back too much, only back this block");
			count = n;
		}
		_read_point -= count;
		_bytes += count;
	}

/* current data cursor */
    size_t get_byte_count()
	{
		return _bytes;
	}
    char* get_read_point()
	{
		return _read_point;
	}

/* cutn and carrayon must appear in pairs and continously */
    bool cutn(size_t count)
	{
		if (count > _bytes) {
			LOG(ALERT, "not enough data to be cutted");
			_cut_remain_bytes = 0;
			//_bytes = _bytes;
			return false;
		}
		_cut_remain_bytes = _bytes - count;
		_bytes = count;
		return true;
	}
    void carrayon()
	{
		_bytes += _cut_remain_bytes;
		_cut_remain_bytes = 0;
		return;
	}

/* store and resume the read point at one time */
	virtual void read_point_cache() = 0;
	virtual bool read_point_resume() = 0;

/* dump and print the infomation or payload */
    virtual void dump_payload(std::string* payload) = 0;
    virtual void print_payload(size_t capacity) = 0;
    virtual void dump_info(std::string* info) = 0;
    virtual void print_info(size_t capacity) = 0;

protected:
	virtual bool init_pool() = 0;

	virtual bool alloc_next_block() = 0;
	virtual bool is_current_block_buf_enough(size_t size, IOBufAllocType type) = 0;
	virtual size_t current_block_remain_buf_size() = 0;
	virtual size_t current_block_alloc_buf_size() = 0;
	virtual char* alloc_from_current_block(size_t n) = 0;
	virtual bool reclaim_to_current_block(size_t n) = 0;

	virtual void move_read_point_to_next_block() = 0;
	virtual size_t current_block_remain_data_size() = 0;
	virtual size_t current_block_consume_data_size() = 0;

protected:
    size_t _block_size;
    size_t _bytes;
    int _blocks/*LE MAX_BLOCKS_NUM*/;
    int _read_block;
    char* _read_point;

	/* for cut and carrayon */
    size_t _cut_remain_bytes;

	/* for read point restore */
	size_t _bytes_record;
	char* _read_point_record;
	int _read_block_record;
	bool _is_read_point_cached;
};

}
#endif
