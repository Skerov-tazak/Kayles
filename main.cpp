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
				std::cerr << "Usage: " << argv[0] << " -r <rules> -a <ip_address> -p <port> -t <timeout>" << std::endl;
				exit(1);
		}
	}

	if (r_arg == nullptr || a_arg == nullptr || p_arg == -1 || t_arg == -1) {
		throw std::invalid_argument("All arguments (-r, -a, -p, -t) are compulsory.");
	}

	// Validate -r: string max length 255 consisting only of 1s and 0s
	size_t r_len = strlen(r_arg);
	if (r_len > 255) {
		throw std::invalid_argument("Rules string (-r) must not exceed 255 characters.");
	}
	if (r_len == 0) {
		throw std::invalid_argument("Rules string (-r) cannot be empty.");
	}
	for (size_t i = 0; i < r_len; ++i) {
		if (r_arg[i] != '0' && r_arg[i] != '1') {
			throw std::invalid_argument("Rules string (-r) must consist only of '0' and '1'.");
		}
	}

	// Validate -a: correct IP address
	struct sockaddr_in sa;
	struct sockaddr_in6 sa6;
	if (inet_pton(AF_INET, a_arg, &(sa.sin_addr)) != 1 &&
		inet_pton(AF_INET6, a_arg, &(sa6.sin6_addr)) != 1) {
		throw std::invalid_argument(std::string("Invalid IP address (-a): ") + a_arg);
	}

	std::cout << "Rules: " << r_arg << std::endl;
	std::cout << "Address: " << a_arg << std::endl;
	std::cout << "Port: " << p_arg << std::endl;
	std::cout << "Timeout: " << t_arg << std::endl;

	// Create the server class
	ServerController mainframe(a_arg, r_arg, p_arg, t_arg);

	return 0;
}
