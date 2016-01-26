#COMAKE2 edit-mode: -*- Makefile -*-
####################64Bit Mode####################
ifeq ($(shell uname -m),x86_64)
CC=gcc
CXX=g++
CXXFLAGS=-g \
  -pipe \
  -W \
  -Wall \
  -fPIC \
  -std=c++11
CFLAGS=-g \
  -pipe \
  -W \
  -Wall \
  -fPIC
CPPFLAGS=-g \
  -O2 \
  -pipe \
  -W \
  -Wall \
  -fPIC \
  -Wno-deprecated \
  -D__const__=
#  -Werror 
INCPATH=-I. \
  -I./include \
  -I../../thirdsrc/gtest-1.7.0/include

comlog_objs=comlog/stderr_log.o \
			comlog/info_log_context.o

io_objs=io/iobuf.o \
		io/iobuf_zero_copy_stream.o

objs=string_printf.o \
     network_util.o \
	 pthread_util.o \
	 timer.o

.PHONY:all
all:libcommon.a UT
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40mall[0m']"
	@echo "make all done"

.PHONY:clean
clean:
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40mclean[0m']"
	rm -rf $(objs)
	make -C comlog clean
	make -C io clean
	rm -rf libcommon.a
	make -C test clean

.PHONY:love
love:
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40mlove[0m']"
	@echo "make love done"

libcommon.a:$(objs) $(comlog_objs) $(io_objs)
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40mlibcommon.a[0m']"
	ar rcs libcommon.a $(objs) $(comlog_objs) $(io_objs)

$(objs): %.o: %.cpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m$@[0m']"
	$(CXX) -c $(INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o $@ $<

$(comlog_objs):
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40mcomlog_objs[0m']"
	make -C comlog

$(io_objs):
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40mio_objs[0m']"
	make -C io

# UT
UT:
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40mtest[0m']"
	make -C test all

endif #ifeq ($(shell uname -m),x86_64)


