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

	static constexpr int BUFFER_SIZE = 100;
	static char* buffer[BUFFER_SIZE];
	static ServerService server_service;
	char* addr;
	int8_t* pawn_template;
	int8_t max_pawn;
	int16_t port;
	static time_t timeout;
	sockaddr_in server_address;

public:
	ServerController(char* addr, char* pawn_template, int16_t port, int8_t timeout);
	~ServerController();
};

#endif // SERVERCONTROLLER_H
