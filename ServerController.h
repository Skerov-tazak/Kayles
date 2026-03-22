#ifndef SERVERCONTROLLER_H
#define SERVERCONTROLLER_H

#include <cstdint>
#include <string>
#include <netinet/in.h>
#include "ServerService.h"
#include "MessageMapper.h"

class ServerController {
	static constexpr int BUFFER_SIZE = 100;
	static char* buffer[BUFFER_SIZE];
	static serverService server_service;
	static messageMapper message_mapper;
	char* addr;
	char* game_cond;
	int16_t port;
	int8_t timeout;
	sockaddr_in server_address;


public:
	ServerController(char* addr, char* game_cond, int16_t port, int8_t timeout);
};

#endif // SERVERCONTROLLER_H
