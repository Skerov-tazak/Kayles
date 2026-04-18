#include "ServerController.h"
#include "MessageMapper.h"
#include "ServerService.h"
#include "kayles_types.h"
#include <cstdint>
#include <system_error>

ServerController::ServerController(char* addr, uint8_t* pawn_template, uint8_t max_pawn, uint16_t port, uint8_t timeout) 
: server_service(timeout, pawn_template, max_pawn), addr_str(addr), port(port) {

	struct addrinfo hints, *res;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET; // Spec says "Wymagana jest obsługa jedynie IPv4"
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_flags = AI_PASSIVE;

	std::string port_str = std::to_string(port);
	int status = getaddrinfo(addr, port_str.c_str(), &hints, &res);
	if (status != 0) {
		throw std::runtime_error(std::string("getaddrinfo: ") + gai_strerror(status));
	}

	socket_fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	if (socket_fd < 0) {
		freeaddrinfo(res);
		throw std::system_error(errno, std::generic_category(), "socket");
	}

	if (bind(socket_fd, res->ai_addr, res->ai_addrlen) < 0) {
		freeaddrinfo(res);
		close(socket_fd);
		throw std::system_error(errno, std::generic_category(), "bind");
	}

	freeaddrinfo(res);
}

ServerController::~ServerController() {
	if (socket_fd >= 0) {
		close(socket_fd);
	}
}

void ServerController::run_server(){
	char buffer[BUFFER_SIZE];
	std::cout << "listening on port " << port << " address " << addr_str << "\n";

	do {
		memset(buffer, 0, sizeof(buffer)); 
		struct sockaddr_in client_address;
		socklen_t address_length = (socklen_t) sizeof(client_address);

		int received_length = recvfrom(socket_fd, buffer, BUFFER_SIZE, 0,
								   (struct sockaddr *) &client_address, &address_length);

		if (received_length < 0) {
			if (errno == EINTR) continue;
			perror("recvfrom");
			continue;
		}

		// Keep raw copy for MSG_WRONG_MSG
		char raw_copy[12];
		memset(raw_copy, 0, 12);
		memcpy(raw_copy, buffer, std::min(12, received_length));

		try {
			// Basic validation before parsing
			if (received_length < 1) throw UKNOWN_MESSAGE_TYPE;

			ClientMessage received = mapper_parse_client_message(buffer);
			server_service.set_message(&received);

			// Type-specific length validation
			if(received.message_type == MSG_JOIN) {
				if(received_length != 5) throw INVALID_MESSAGE_LENGTH;
			} else if(received.message_type == MSG_MOVE_1 || received.message_type == MSG_MOVE_2) {
				if(received_length != 10) throw INVALID_MESSAGE_LENGTH;
			} else if(received.message_type == MSG_KEEP_ALIVE || received.message_type == MSG_GIVE_UP) {
				if(received_length != 9) throw INVALID_MESSAGE_LENGTH;
			} else {
				throw UKNOWN_MESSAGE_TYPE;
			}

			std::cout << "Received message: " << received.to_string() << "\n";
			std::fflush(stdout);
			server_service.perform_requests();

			// Success: send GAME_STATE
			GameState* relevant_state = server_service.get_relevant_gamestate();
			if (relevant_state) {
				memset(buffer, 0, sizeof(buffer));
				mapper_encode_gamestate_message(relevant_state, buffer);
				size_t message_length = 14 + (relevant_state->get_max_pawn() / 8 + 1);
				sendto(socket_fd, buffer, message_length, 0, (struct sockaddr *) &client_address, address_length);
			}

		} catch (const ErrorType& error_index) {
			if(error_index == ILLEGAL_MOVE) {
				// Illegal move -> send CURRENT game state
				GameState* relevant_state = server_service.get_relevant_gamestate();
				if (relevant_state) {
					memset(buffer, 0, sizeof(buffer));
					mapper_encode_gamestate_message(relevant_state, buffer);
					size_t message_length = 14 + (relevant_state->get_max_pawn() / 8 + 1);
					sendto(socket_fd, buffer, message_length, 0, (struct sockaddr *) &client_address, address_length);
				}
			} else {
				// MSG_WRONG_MSG
				memset(buffer, 0, sizeof(buffer));
				memcpy(buffer, raw_copy, 12);
				buffer[12] = 255; // Status
				if (error_index == INVALID_MESSAGE_LENGTH) {
					buffer[13] = (uint8_t)std::min(10, received_length); // Indeks bajtu
				} else if (error_index == UKNOWN_MESSAGE_TYPE) {
					buffer[13] = 0; // First byte is unknown
				} else if (error_index == INVALID_PLAYER) {
					buffer[13] = 1; // player_id starts at 1
				} else if (error_index == INVALID_GAME_ID) {
					buffer[13] = 5; // game_id starts at 5
				} else {
					buffer[13] = 0;
				}
				sendto(socket_fd, buffer, 14, 0, (struct sockaddr *) &client_address, address_length);
			}
		} catch (const std::bad_alloc& e) {
			// Spec: ignore MSG_JOIN if out of memory
		}

	} while(true);
}
