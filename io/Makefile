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
  -I../ \
  -I../../thirdsrc/gtest-1.7.0/include
objs=iobuf.o \
	 iobuf_zero_copy_stream.o

.PHONY:all
all:$(objs)
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40mall[0m']"
	@echo "make all done"

.PHONY:clean
clean:
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40mclean[0m']"
	rm -rf $(objs)

.PHONY:love
love:
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40mlove[0m']"
	@echo "make love done"

$(objs): %.o: %.cpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m$@[0m']"
	$(CXX) -c $(INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o $@ $<

#test:test_info_log_context.cpp $(objs)
#	@echo "[[1;32;40mbuild test_info_log_context[0m]"
#	$(CXX) $(INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o test_info_log_context $^ ../timer.o ../string_printf.o


endif #ifeq ($(shell uname -m),x86_64)


