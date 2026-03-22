#include <cstdint>
#include <iostream>
#include <stdint.h>
#include <string>
#include <cstring>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <errno.h>
#include <stdlib.h>
#include "err.h"
#include "ServerService.h"
#include "ServerController.h"

ServerController::ServerController(char* addr, char* game_cond, int16_t port, int8_t timeout) {
	this->addr = addr;
	this->game_cond = game_cond;
	this->port = port;
	this->timeout = timeout;

	int socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
	if (socket_fd < 0) {
		syserr("cannot create a socket");
	}

	server_address.sin_family = AF_INET; // IPv4
	server_address.sin_port = htons(port);
	if (inet_pton(AF_INET, addr, &(server_address.sin_addr)) <= 0)
		fatal("Incorrect IP adress");

	if (bind(socket_fd, (struct sockaddr *) &server_address, (socklen_t) sizeof(server_address)) < 0) {
		syserr("bind");
	}

	std::cout << "listening on port " << port << " addres " << addr << "\n";
}

