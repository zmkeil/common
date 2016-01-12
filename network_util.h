
/***********************************************
  File name		: network_util.h
  Create date	: 2016-01-06 19:29
  Modified date : 2016-01-06 19:33
  Author		: zmkeil, alibaba.inc
  Express : 
  
 **********************************************/

#ifndef ZMKEIL_NETWORK_UTIL_H
#define ZMKEIL_NETWORK_UTIL_H
extern "C" {
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <strings.h>
#include <string.h>
}

namespace common {

bool sockaddr_init(struct sockaddr_in* addr, socklen_t addr_len, const char* ip, int port);

bool connect_with_timeout(int sockfd, struct sockaddr* addr, socklen_t addr_len, int timeout);

int send_with_timeout(int sockfd, char* source, size_t size, int timeout);

int recv_with_timeout(int sockfd, char* dst, size_t size, int timeout);

bool is_socket_clear_and_idle(int sockfd); 

}
#endif
