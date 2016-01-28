#ifndef COMMON_IO_ABSTRACT_IOBUF_H
#define COMMON_IO_ABSTRACT_IOBUF_H

#include <stdio.h>
#include <string.h>
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
			_blocks(0),
			_read_block(0),
			_read_point(nullptr),
			_bytes(0),
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
		if (!_read_point || (_bytes == 0)) {
			LOG(INFO, "no more data in zero_copy_input_stream");
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
    void read_point_cache()
    {
    	_read_point_record = _read_point;
        _read_block_record = _read_block;
    	_bytes_record = _bytes;
    	_is_read_point_cached = true;
    }

    bool read_point_resume()
    {
    	if (!_is_read_point_cached) {
    		LOG(ALERT, "read point not cached");
    		return false;
    	}
    	// first release all current data
    	release_all();
    	// then copy the origin data to new point, we can't simplly reclaim because that several
    	// alloc/read operation may be called after last read_point_cache()
    	// this will be a continuely malloc buf.
    	char* read_point_new = NULL;
        int read_block_new = _blocks - 1;
    	if (alloc(&read_point_new, _bytes_record, IOBUF_ALLOC_EXACT) != (int)_bytes_record) {
    		LOG(ALERT, "alloc _bytes_record buf failed");
    		return false;
    	}
    	// resume the last cache scene
    	_read_point = _read_point_record;
        _read_block = _read_block_record;
    	_bytes = _bytes_record;
    	// copy data
        char* tmp_buf = read_point_new;
    	while (_bytes > 0) {
    		const char* read_buf = NULL;
    		int read_len = read(&read_buf);
    		memcpy(tmp_buf, read_buf, read_len);
            tmp_buf += read_len;
    	}
    	// set the new scene
    	_read_point = read_point_new;
        _read_block = read_block_new;
    	_bytes = _bytes_record;
    	_is_read_point_cached = false;
    	return true;
    }

/* dump and print the infomation or payload */
    virtual void dump_payload(std::string* payload) = 0;
    virtual void print_payload(size_t capacity) = 0;
    virtual void dump_info(std::string* info) = 0;
    virtual void print_info(size_t capacity) = 0;

protected:
    // three point:
    //   1.init the pool struct
    //   2.malloc the first block, and you could also malloc severy blocks
    //     set _blocks = 1
    //   3.set _read_block = _block - 1
    //   4.set the _read_point at the start of first block
	virtual bool init_pool() = 0;
    // two point:
    //   1.get a new block,
    //   2._blocks++
    //   3.some additional operation such as NgxplusIOBuf's _pool->current moving
    //     this is just for Convenience, commonly it can be got with _blocks
	virtual bool alloc_next_block() = 0;
    // check if the current block has buf GE size
	virtual bool is_current_block_buf_enough(size_t size, IOBufAllocType type) = 0;
    // just calculate current block's remained buf size, zero or a positive
	virtual size_t current_block_remain_buf_size() = 0;
    // just calculate current block's alloced buf size
	virtual size_t current_block_alloc_buf_size() = 0;
    // alloc n bytes from current block, Don't need to consider security
	virtual char* alloc_from_current_block(size_t n) = 0;
    // reclaim n bytes to current block, Don't need to consider security
	virtual bool reclaim_to_current_block(size_t n) = 0;

    // two point:
    //   1.move _read_point to next block
    //   2._read_block++, the current_read_block cat be got with _read_block in
    //     all implements (include NgxplusIOBuf)
	virtual void move_read_point_to_next_block() = 0;
    // just calculate current block's remain data size, zero or a positive
    // don't need to consider _bytes
	virtual size_t current_block_remain_data_size() = 0;
    // just calculate current block's consume data size
	virtual size_t current_block_consume_data_size() = 0;

protected:
    size_t _block_size;
    // _blocks, _read_block, _read_point is operated in protected method
    int _blocks/*count*/;
    int _read_block/*index: 0 ~ xx*/;
    char* _read_point;

private:
    size_t _bytes;
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
