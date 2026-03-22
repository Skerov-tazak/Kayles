#ifndef SERVERCONTROLLER_H
#define SERVERCONTROLLER_H

#include <cstdint>
#include <netinet/in.h>
#include <iostream>
#include <stdexcept>
#include <stdint.h>
#include <cstring>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdlib.h>

#include "ServerService.h"
#include "MessageMapper.h"

class ServerController {

	private:

		static constexpr int BUFFER_SIZE = 10;
		static char buffer[BUFFER_SIZE];
		ServerService server_service;
		int socket_fd;
		char* addr;
		uint16_t port;
		sockaddr_in server_address;

		void run_server();

	public:
		ServerController(char* addr, uint8_t* pawn_template, uint8_t max_pawn, uint16_t port, uint8_t timeout);
		~ServerController();
};

#endif // SERVERCONTROLLER_H
