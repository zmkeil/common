
#include "comlog/info_log_context.h"
#include "network_util.h"

namespace common {

#define MAX_FD_DESCRIPTOR_VALUE 100

// by default, use AF_INET
bool sockaddr_init(struct sockaddr_in* addr, socklen_t addr_len, const char* ip, int port)
{
    if (!addr) {
        LOG(ALERT, "sockaddr init error: addr is null");
        return false;
    }
    bzero(addr, addr_len);
    addr->sin_family = AF_INET;
    addr->sin_port = htons(port);
    int ret = inet_pton(AF_INET, ip, &addr->sin_addr);
    if (ret <= 0) {
        LOG(ALERT, "sockaddr init error: wrong ip format \"%s\"", ip);
        return false;
    }
    return true; 
}

/*
 * use the traddional set-non-block -- select -- set-back method
 * connect with the non-block socket may result in:
 *  I) the peer don't open this listen port, the behavior is different based on OS. As described below:
 *     1.return -1, EINPROCESS(wait for SYN-ACK), but then recv RST, then the so_error be setted ECONNREFUSED,
 *       and select() return immediately with the flag setted. then we must use getsockopt to get SO_ERROR
 *     2.return -1, EINPROCESS(wait for SYN-ACK), the peer just drop it. then select() return when timeout
 *       without setted flag
 *  II) the peer host don't exist
 *     1.return -1, ENETUNREACH(this is guaranteed by lower protocol)
 *
 * the SO_ERROR, refer to http://blog.csdn.net/woowenjie/article/details/5235976
 */
bool connect_with_timeout(int sockfd, struct sockaddr* addr, socklen_t addr_len, int timeout)
{
    struct timeval timeval = {timeout, 0};

    // set the sockfd in non-blocking
    int flags = fcntl(sockfd, F_GETFL);
    if (fcntl(sockfd, F_SETFL, flags | O_NONBLOCK)) {
        LOG(ALERT, "connect error: fcntl failed %s", strerror(errno));
		close(sockfd);
        return false;
    }

    if (connect(sockfd, addr, addr_len) == 0) {
        return true;
    }
    if (errno != EINPROGRESS) {
        LOG(ALERT, "connect error: %s", strerror(errno));
		close(sockfd);
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
	bool ret = true;
    if(FD_ISSET(sockfd, &write_set)) {
		// check the SO_ERROR, whether recv RST or other errors
		int optval;
		socklen_t optval_len = sizeof(int);
		getsockopt(sockfd, SOL_SOCKET, SO_ERROR, (char*)&optval, &optval_len);
		switch (optval) {
			case 0:
				break;
			case ECONNREFUSED:
			case ECONNRESET:
				LOG(ALERT, "connect error: %s", strerror(optval));
				ret = false;
				break;
			default:
				LOG(ALERT, "connect error: %s", strerror(optval));
				ret = false;
				break;
		}
    } else {
		LOG(ALERT, "connect error: timeout");
		ret = false;
	}

    // reset the flags
    flags = fcntl(sockfd, F_GETFL);
    if (fcntl(sockfd, F_SETFL, flags & ~O_NONBLOCK)) {
        LOG(ALERT, "connect error: fcntl failed %s", strerror(errno));
        ret = false;
    }

	// if error, close the sockfd
	if (!ret) {
		close(sockfd);
	}
    return ret;
}

int send_with_timeout(int sockfd, char* source, size_t size, int timeout)
{
    struct timeval send_timeout = {timeout, 0};
    if (setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO, (const char*)&send_timeout, sizeof(send_timeout)) != 0) {
        LOG(ALERT, "send error: setscokopt timeout failed %s", strerror(errno));
        return -1;
    }

    int ret = send(sockfd, source, size, 0);
    if (ret < 0) {
        LOG(ALERT, "send error: %s", strerror(errno));
    }
    return ret;
}

int recv_with_timeout(int sockfd, char* dst, size_t size, int timeout)
{
    struct timeval recv_timeout = {timeout, 0};
    if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&recv_timeout, sizeof(recv_timeout)) != 0) {
        LOG(ALERT, "recv error: setscokopt timeout failed %s", strerror(errno));
        return -1;
    }

    int ret = recv(sockfd, dst, size, 0);
    if (ret < 0) {
        LOG(ALERT, "recv error: %s", strerror(errno));
    }
    return ret;
}

bool is_socket_clear_and_idle(int sockfd)
{
    // set the sockfd in non-blocking
    int flags = fcntl(sockfd, F_GETFL);
    if (fcntl(sockfd, F_SETFL, flags | O_NONBLOCK)) {
        LOG(ALERT, "check clear error: fcntl failed %s", strerror(errno));
        return false;
    }

	char val;
	int n = recv(sockfd, &val, 1, 0);
    bool ret = false;
	if (n == 0) {
		LOG(ALERT, "check clear note: recv fin or rst");
	}
	if (n == 1) {
		LOG(ALERT, "check clear note: connection filled with data");
	}
	if (n < 0) {
		if ((errno == EAGAIN) || (errno == EWOULDBLOCK)) {
			ret = true;
		} else {
			// ECONNRESET
            LOG(ALERT, "check clear note: unexpected state %s", strerror(errno));
        }
	}

    // reset the flags
    flags = fcntl(sockfd, F_GETFL);
    if (fcntl(sockfd, F_SETFL, flags & ~O_NONBLOCK)) {
        LOG(ALERT, "check clear error: fcntl failed %s", strerror(errno));
        return false;
    }
	return ret;
}

}
