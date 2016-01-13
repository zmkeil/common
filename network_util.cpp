
#include "common.h"

namespace common {

#define MAX_FD_DESCRIPTOR_VALUE 100

// by default, use AF_INET
bool sockaddr_init(struct sockaddr_in* addr, socklen_t addr_len, const char* ip, int port)
{
    if (!addr) {
        CLOG(CALERT, "addr is null\n");
        return false;
    }
    bzero(addr, addr_len);
    addr->sin_family = AF_INET;
    addr->sin_port = htons(port);
    int ret = inet_pton(AF_INET, ip, &addr->sin_addr);
    if (ret <= 0) {
        CLOG(CALERT, "error ip format \"%s\"\n", ip);
        return false;
    }
    return true; 
}

// use the traddional set-non-block -- select -- set-back method
// if you want keep the sockfd as block, use this method
bool connect_with_timeout(int sockfd, struct sockaddr* addr, socklen_t addr_len, int timeout)
{
    struct timeval timeval = {timeout, 0};

    // set the sockfd in non-blocking
    int flags = fcntl(sockfd, F_GETFL);
    if (fcntl(sockfd, F_SETFL, flags | O_NONBLOCK)) {
        CLOG(CALERT, "fcntl failed with error: %s\n", strerror(errno));
        return false;
    }

    if (connect(sockfd, addr, addr_len) == 0) {
        return true;
    }
    if (errno != EINPROGRESS) {
        CLOG(CALERT, "connect error: %s\n", strerror(errno));
        return false;
    }

    // reset the flags
    flags = fcntl(sockfd, F_GETFL);
    if (fcntl(sockfd, F_SETFL, flags & ~O_NONBLOCK)) {
        CLOG(CALERT, "fcntl failed with error: %s\n", strerror(errno));
        return false;
    }

    fd_set write_set, err_set;
    FD_ZERO(&write_set);
    FD_ZERO(&err_set);
    FD_SET(sockfd, &write_set);
    FD_SET(sockfd, &err_set);

    // check if the sockfd is ready, select will block timeval
    // the nfds is the max-numbered of all fd-descriptor, if you open many
    // other files before, here will be wrong
    select(MAX_FD_DESCRIPTOR_VALUE/*nfds*/, NULL, &write_set, &err_set, &timeval);
    if(FD_ISSET(sockfd, &write_set)) {
        // TODO: connect error??
        CLOG(CWARN, "connect done\n");
        return true;
    }
    CLOG(CALERT, "connect timeout\n");
    return false;
}

int send_with_timeout(int sockfd, char* source, size_t size, int timeout)
{
    struct timeval send_timeout = {timeout, 0};
    if (setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO, (const char*)&send_timeout, sizeof(send_timeout)) != 0) {
        CLOG(CALERT, "setscokopt send timeout error: %s\n", strerror(errno));
        return -1;
    }

    int ret = send(sockfd, source, size, 0);
    if (ret < 0) {
        CLOG(CALERT, "send error: %s\n", strerror(errno));
    }
    return ret;
}

int recv_with_timeout(int sockfd, char* dst, size_t size, int timeout)
{
    struct timeval recv_timeout = {timeout, 0};
    if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&recv_timeout, sizeof(recv_timeout)) != 0) {
        CLOG(CALERT, "setscokopt recv timeout error: %s\n", strerror(errno));
        return -1;
    }

    int ret = recv(sockfd, dst, size, 0);
    if (ret < 0) {
        CLOG(CALERT, "recv error: %s\n", strerror(errno));
    }
    return ret;
}

bool is_socket_clear_and_idle(int sockfd)
{
    // set the sockfd in non-blocking
    int flags = fcntl(sockfd, F_GETFL);
    if (fcntl(sockfd, F_SETFL, flags | O_NONBLOCK)) {
        CLOG(CALERT, "fcntl failed with error: %s\n", strerror(errno));
        return false;
    }

	char val;
	int n = recv(sockfd, &val, 1, 0);
    bool ret = false;
	if (n == 0) {
		CLOG(CALERT, "recv fin or rst: %s\n", strerror(errno));
	}
	if (n == 1) {
		CLOG(CALERT, "connection filled with data");
	}
	if (n < 0) {
		if ((errno == EAGAIN) || (errno == EWOULDBLOCK)) {
			ret = true;
		} else {
            CLOG(CALERT, "unexpected error: %s\n", strerror(errno));
        }
	}

    // reset the flags
    flags = fcntl(sockfd, F_GETFL);
    if (fcntl(sockfd, F_SETFL, flags & ~O_NONBLOCK)) {
        CLOG(CALERT, "fcntl failed with error: %s\n", strerror(errno));
        return false;
    }
	return ret;
}

}
