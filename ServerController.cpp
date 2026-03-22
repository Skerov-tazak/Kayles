#include "ServerController.h"

ServerController::ServerController(char* addr, char* game_cond, int16_t port, int8_t timeout) 
: addr(addr), game_cond(game_cond), port(port) {

	this->timeout = (time_t)timeout;
	server_service(game_cond);
	

	int socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
	if (socket_fd < 0) {
		throw std::system_error();
	}

	server_address.sin_family = AF_INET; // IPv4
	server_address.sin_port = htons(port);
	if (inet_pton(AF_INET, addr, &(server_address.sin_addr)) <= 0)
		throw std::invalid_argument("Incorrect IP adress");

	if (bind(socket_fd, (struct sockaddr *) &server_address, (socklen_t) sizeof(server_address)) < 0) {
		throw std::system_error();
	}

	std::cout << "listening on port " << port << " addres " << addr << "\n";
}

