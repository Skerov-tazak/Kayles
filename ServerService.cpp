#include <cstdint>
#include <stdint.h>
#include <cstring>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include "err.h"
#include "ServerService.h"
#include "GameState.h"

ClientMessage serverService::parse_message(){
    // Placeholder implementation
    return ClientMessage();
}

void serverService::call_functions() {
	switch (latest_message.message_type) {
		case MSG_JOIN:
			// Create Game OR change openGame state to TURN_B
			break;
		case MSG_GIVE_UP:
			// Change state to WIN_A or WIN_B and keeps the state for timeout seconds 
			break;
		case MSG_MOVE_1:
			break;
		case MSG_MOVE_2:
			break;
		case MSG_KEEP_ALIVE:
			break;
		default:
			fatal("Unrecognised message code");
	}
}
