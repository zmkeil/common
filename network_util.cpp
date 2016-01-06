extern "C" {
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <strings.h>
#include <string.h>
}
#include "common.h"

namespace common {

// by default, use AF_INET
bool sockaddr_init(struct sockaddr_in* addr, socklen_t addr_len, const char* ip, int port)
{
    if (!addr) {
        LOG(NGX_LOG_LEVEL_ALERT, "addr is null");
        return false;
    }
    bzero(addr, addr_len);
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    int ret = inet_pton(AF_INET, ip, &addr.sin_addr);
    if (ret <= 0) {
        LOG(NGX_LOG_LEVEL_ALERT, "error ip format \"%s\"", ip);
        return false;
    }
    return true; 
}

// use the traddional set-non-block -- select -- set-back method
// if you want keep the socket as block, use this method
bool connect_with_timeout(int socket, struct sockaddr* addr, socklen_t addr_len, int timeout)
{
    struct timeval timeval = {timeout, 0};

    // set the socket in non-blocking
    int flags = fcntl(socket, F_GETFL);
    if (fcntl(socket, F_SETFL, flags | O_NONBLOCK)) {
        LOG(NGX_LOG_LEVEL_ALERT, "fcntl failed with error: %d\n", errno);
        return false;
    }

    if (connect(socket, addr, addr_len) == 0) {
        return true;
    }
    if (errno != EINPROGRESS) {
        LOG(NGX_LOG_LEVEL_ALERT, "connect error: %d\n", errno);
        return false;
    }

    // reset the flags
    flags = fcntl(socket, F_GETFL);
    if (fcntl(socket, F_SETFL, flags & ~O_NONBLOCK)) {
        LOG(NGX_LOG_LEVEL_ALERT, "fcntl failed with error: %d\n", errno);
        return false;
    }

    fd_set write_set, err_set;
    FD_ZERO(&write_set);
    FD_ZERO(&err_set);
    FD_SET(socket, &write_set);
    FD_SET(socket, &err_set);

    // check if the socket is ready, select will block timeval
    // the nfds is the max-numbered of all fd-descriptor, if you open many 
    // other files before, here will be wrong
    select(10/*nfds*/, NULL, &write_set, &err_set, &timeval);
    if(FD_ISSET(socket, &write_set)) {
        return true;
    }
    return false;
}

// In practices, connect is followed by send, and OS provide timeout mechanism for send-system-call,
int connect_and_send_with_timeout(int socket, struct sockaddr* addr, socklen_t addr_len,
        char* source, size_t size, int timeout)
{
    // set the socket in non-blocking
    int flags = fcntl(socket, F_GETFL);
    if (fcntl(socket, F_SETFL, flags | O_NONBLOCK)) {
        LOG(NGX_LOG_LEVEL_ALERT, "fcntl failed with error: %d\n", errno);
        return -1;
    }

    if ((connect(socket, addr, addr_len) != 0) &&
            (errno != EINPROGRESS)) {
        LOG(NGX_LOG_LEVEL_ALERT, "connect error: %d\n", errno);
        return false;
    }

    // reset the flags
    // keep the socket to be block, then send with timeout
    flags = fcntl(socket, F_GETFL);
    if (fcntl(socket, F_SETFL, flags & ~O_NONBLOCK)) {
        LOG(NGX_LOG_LEVEL_ALERT, "fcntl failed with error: %d\n", errno);
        return -1;
    }

    struct timeval send_timeout = {timeout, 0};
    if (setsockopt(socket,SOL_SOCKET,SO_SNDTIMEO,(const char*)&send_timeout,sizeof(read_timeout)) != 0) {
        LOG(NGX_LOG_LEVEL_ALERT, "setscokopt send timeout error: %d\n", errno);
        return -1;
    }

    int ret = send(sockfd, source, size, 0);
    if (ret < 0) {
        LOG(NGX_LOG_LEVEL_ALERT, "send error: %d\n", errno);
    }
    return ret;
}

int recv_with_timeout(int socket, char* dst, size_t size, int timeout)
{
    struct timeval recv_timeout = {timeout, 0};
    if (setsockopt(socket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&recv_timeout, sizeof(recv_timeout)) != 0) {
        LOG(NGX_LOG_LEVEL_ALERT, "setscokopt recv timeout error: %d\n", errno);
        return -1;
    }

    int ret = recv(sockfd, dst, size, 0);
    if (ret < 0) {
        LOG(NGX_LOG_LEVEL_ALERT, "recv error: %d\n", errno);
    }
    return ret;
}

}
