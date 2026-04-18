#ifndef SERVERCONTROLLER_H
#define SERVERCONTROLLER_H

#include <cstdint>
#include <netinet/in.h>
#include <iostream>
#include <stdexcept>
#include <stdint.h>
#include <cstring>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdlib.h>

#include "ServerService.h"
#include "MessageMapper.h"

class ServerController {

	private:

		static constexpr int BUFFER_SIZE = 1024;
		ServerService server_service;
		int socket_fd;
		std::string addr_str;
		uint16_t port;

	public:
		ServerController(char* addr, uint8_t* pawn_template, uint8_t max_pawn, uint16_t port, uint8_t timeout);
		
		~ServerController();

		void run_server();
};

#endif // SERVERCONTROLLER_H
