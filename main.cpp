#include <exception>
#include <iostream>
#include <stdint.h>
#include <string>
#include <cstring>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <errno.h>
#include <stdlib.h>
#include "ServerController.h"

long parse_long(const char* input, long min, long max, const std::string& name) {
	char* endptr;
	errno = 0;
	long val = strtol(input, &endptr, 10);
	if (errno != 0 || *endptr != '\0' || val < min || val > max) {
		throw std::invalid_argument("Invalid " + name + ": " + input);
	}
	return val;
}

uint8_t* string_to_bitstring(const char* s, size_t len) {
	size_t byte_count = (len + 7) / 8;
	uint8_t* bits = new uint8_t[byte_count];
	std::memset(bits, 0, byte_count);
	for (size_t i = 0; i < len; ++i) {
		if (s[i] == '1') {
			bits[i / 8] |= (1 << (7 - (i % 8)));
		} else if (s[i] != '0') {
			delete[] bits;
			throw std::invalid_argument("Rules string must consist only of '0' and '1'.");
		}
	}
	return bits;
}

int main(int argc, char* argv[]) {
	char *r_arg = nullptr;
	char *a_arg = nullptr;
	int p_arg = -1;
	int t_arg = -1;

	int opt;
	while ((opt = getopt(argc, argv, "r:a:p:t:")) != -1) {
		switch (opt) {
			case 'r':
				r_arg = optarg;
				break;
			case 'a':
				a_arg = optarg;
				break;
			case 'p':
				p_arg = (int)parse_long(optarg, 0, 65535, "port");
				break;
			case 't':
				t_arg = (int)parse_long(optarg, 1, 99, "timeout");
				break;
			default:
				std::cout << "Usage: " << argv[0] << " -r <rules> -a <ip_address> -p <port> -t <timeout>" << std::endl;
				exit(1);
		}
	}

	if (r_arg == nullptr || a_arg == nullptr || p_arg == -1 || t_arg == -1) {
		std::cout << "Usage: " << argv[0] << " -r <rules> -a <ip_address> -p <port> -t <timeout>" << std::endl;
		exit(1);
	}

	// Validate -r: string max length 255 consisting only of 1s and 0s
	size_t r_len = strlen(r_arg);
	if (r_len > 256) {
		throw std::invalid_argument("Rules string (-r) must not exceed 255 characters.");
	}
	if (r_len == 0) {
		throw std::invalid_argument("Rules string (-r) cannot be empty.");
	}
	if (r_arg[r_len - 1] != '1') {
		throw std::invalid_argument("Rules string (-r) must end with '1'.");
	}
	for (size_t i = 0; i < r_len; ++i) {
		if (r_arg[i] != '0' && r_arg[i] != '1') {
			throw std::invalid_argument("Rules string (-r) must consist only of '0' and '1'.");
		}
	}

	// Create bitstring for rules
	uint8_t* r_bitstring = string_to_bitstring(r_arg, r_len);
	uint8_t max_pawn = (uint8_t)(r_len - 1);

	// Create the server class
	try {

		ServerController mainframe(a_arg, r_bitstring, max_pawn,
				(uint16_t)p_arg, (uint8_t)t_arg);
		
		mainframe.run_server();

	} catch (const std::bad_alloc& e) {
		std::cerr << "Not Enough Memory to create necessary objects! " << e.what() << "\n";
	} catch (const std::exception& e) {
		std::cerr << "Critical Server Error: " << e.what() << "\n";
	}
 
	delete[] r_bitstring;

	return 0;
}

