#include <iostream>
#include <string>
#include <cstring>
#include <vector>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/time.h>

#include "kayles_types.h"
#include "GameState.h"
#include "MessageMapper.h"

void print_usage() {
	std::cerr << "Usage: ./client -a address -p port -m type/player_id/game_id/pawn -t timeout\n";
}

std::string status_to_string(Status s) {
	switch (s) {
		case WAITING_FOR_OPPONENT: return "WAITING_FOR_OPPONENT";
		case TURN_A: return "TURN_A";
		case TURN_B: return "TURN_B";
		case WIN_A: return "WIN_A";
		case WIN_B: return "WIN_B";
		default: return "UNKNOWN";
	}
}

std::string error_to_string(ErrorType e) {
	switch (e) {
		case UKNOWN_MESSAGE_TYPE: return "UKNOWN_MESSAGE_TYPE";
		case INVALID_PLAYER: return "INVALID_PLAYER";
		case INVALID_GAME_ID: return "INVALID_GAME_ID";
		case INVALID_PAWN: return "INVALID_PAWN";
		default: return "UNKNOWN_ERROR";
	}
}

int main(int argc, char* argv[]) {
	std::string address_str;
	int port = -1;
	std::string message_str;
	int timeout_sec = -1;

	int opt;
	while ((opt = getopt(argc, argv, "a:p:m:t:")) != -1) {
		switch (opt) {
			case 'a':
				address_str = optarg;
				break;
			case 'p':
				port = std::stoi(optarg);
				break;
			case 'm':
				message_str = optarg;
				break;
			case 't':
				timeout_sec = std::stoi(optarg);
				break;
			default:
				print_usage();
				return 1;
		}
	}

	if (address_str.empty() || port == -1 || message_str.empty() || timeout_sec == -1) {
		print_usage();
		return 1;
	}

	// Parse message: type/player_id/game_id/pawn
	uint32_t msg_type, player_id, game_id, pawn;
	if (sscanf(message_str.c_str(), "%u/%u/%u/%u", &msg_type, &player_id, &game_id, &pawn) != 4) {
		std::cerr << "Invalid message format. Use type/player_id/game_id/pawn\n";
		return 1;
	}

	ClientMessage msg;
	msg.message_type = static_cast<MessageType>(msg_type);
	msg.player_id = player_id;
	msg.game_id = game_id;
	msg.pawn = static_cast<uint8_t>(pawn);

	// Networking
	int socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
	if (socket_fd < 0) {
		perror("socket");
		return 1;
	}

	// Resolve address
	struct addrinfo hints, *res;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_DGRAM;
	if (getaddrinfo(address_str.c_str(), std::to_string(port).c_str(), &hints, &res) != 0) {
		perror("getaddrinfo");
		close(socket_fd);
		return 1;
	}

	// Set timeout
	struct timeval tv;
	tv.tv_sec = timeout_sec;
	tv.tv_usec = 0;
	if (setsockopt(socket_fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0) {
		perror("setsockopt");
		freeaddrinfo(res);
		close(socket_fd);
		return 1;
	}

	// Encode and send
	char buffer[1024];
	memset(buffer, 0, sizeof(buffer));
	mapper_encode_client_message(&msg, buffer);

	if (sendto(socket_fd, buffer, 10, 0, res->ai_addr, res->ai_addrlen) < 0) {
		perror("sendto");
		freeaddrinfo(res);
		close(socket_fd);
		return 1;
	}

	// Receive
	struct sockaddr_in from_addr;
	socklen_t from_len = sizeof(from_addr);
	ssize_t received_len = recvfrom(socket_fd, buffer, sizeof(buffer), 0, (struct sockaddr*)&from_addr, &from_len);

	if (received_len < 0) {
		if (errno == EAGAIN || errno == EWOULDBLOCK) {
			std::cout << "No response from server within timeout.\n";
		} else {
			perror("recvfrom");
		}
		freeaddrinfo(res);
		close(socket_fd);
		return 0;
	}

	// Check for error
	// According to ServerController, error is indicated by byte 13 being 255
	if (received_len >= 15 && static_cast<uint8_t>(buffer[13]) == 255) {
		ErrorType error_code = static_cast<ErrorType>(buffer[14]);
		std::cout << "Server returned error: " << error_to_string(error_code) << " (code " << (int)error_code << ")\n";
	} else if (received_len >= 14) {
		// Parse GameState
		GameState state = mapper_parse_gamestate_message(buffer);
		
		std::cout << "--- Game State ---\n";
		std::cout << "Game ID: " << state.get_game_id() << "\n";
		std::cout << "Player A ID: " << state.get_player_a_id() << "\n";
		std::cout << "Player B ID: " << state.get_player_b_id() << "\n";
		std::cout << "Status: " << status_to_string(state.get_status()) << "\n";
		std::cout << "Max Pawn: " << (int)state.get_max_pawn() << "\n";
		
		std::cout << "Board: ";
		uint8_t* row = state.get_pawn_row();
		for (int i = 0; i < state.get_max_pawn(); ++i) {
			int byte_idx = i / 8;
			int bit_idx = i % 8;
			if (row[byte_idx] & (1 << bit_idx)) {
				std::cout << "I";
			} else {
				std::cout << ".";
			}
		}
		std::cout << "\n";
	} else {
		std::cout << "Received invalid or short message from server.\n";
	}

	freeaddrinfo(res);
	close(socket_fd);
	return 0;
}
