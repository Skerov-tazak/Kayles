#include "ServerController.h"
#include "MessageMapper.h"
#include "ServerService.h"
#include "kayles_types.h"
#include <cstdint>
#include <exception>
#include <system_error>

ServerController::ServerController(char* addr, uint8_t* pawn_template, uint8_t max_pawn, uint16_t port, uint8_t timeout) 
: addr(addr), server_service(timeout, pawn_template, max_pawn), port(port) {

	socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
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
}

void ServerController::run_server(){

	std::cout << "listening on port " << port << " addres " << addr << "\n";

	do {

		// Clean the buffer.
		memset(buffer, 0, sizeof(buffer)); 
		int flags = 0;

		struct sockaddr_in client_address;
		socklen_t address_length = (socklen_t) sizeof(client_address);

		// Receive message

		int received_length = recvfrom(socket_fd, buffer, BUFFER_SIZE, flags,
								   (struct sockaddr *) &client_address, &address_length);

		if (received_length < 0) {
			throw std::system_error();
		}

		size_t message_length = 0;

		// Parse message and perform action 
		// Parsing is always safe (everybitstring can be mapped to int ...)
		ClientMessage received = mapper_parse_client_message(buffer);
		server_service.set_message(&received);
		
		try {

			// This will except when bad data 
			server_service.perform_requests();

			// encode the gamestate to buffer and send it 
			memset(buffer, 0, sizeof(buffer)); 
			mapper_encode_gamestate_message(server_service.get_relevant_gamestate(), buffer);

		} catch (ErrorType error_index) {
			
			// Use error data to send exception to client
			memset(buffer, 0, sizeof(buffer)); 
			mapper_encode_client_message(&received, buffer);
			buffer[13] = 255;
			buffer[14] = error_index;
		}

		// Send buffer (either error or gamestate)
		ssize_t sent_length = sendto(socket_fd, buffer, message_length, flags,
									 (struct sockaddr *) &client_address, address_length);
		if (sent_length < 0) {
			throw std::system_error();
		}

	} while(true);
}
