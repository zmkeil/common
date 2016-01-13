
/***********************************************
  File name		: test_network.cpp
  Create date	: 2016-01-12 17:10
  Modified date : 2016-01-12 17:14
  Author		: zmkeil, alibaba.inc
  Express : 
  
 **********************************************/
#include <iostream>
#include "common.h"

#define PORT 8899

int main(int argc, char* argv[])
{
	if (argc != 2) {
		std::cout << "Usage: ./test_network <IP>" << std::endl;
		exit(-1);
	}
	common::COMMON_TEST_FLAG = 1;

	struct sockaddr_in servaddr;
	char* ip = argv[1];
	int port = PORT;

	common::sockaddr_init((struct sockaddr_in*)&servaddr, sizeof(servaddr), ip, port);

	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	common::connect_with_timeout(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr), 1);

	while (common::is_socket_clear_and_idle(sockfd)) {
		std::cout << "establish" << std::endl;
		sleep(1);
	}

	std::cout << "broken" << std::endl;
	return 0;
}