#ifndef SERVERSERVICE_H
#define SERVERSERVICE_H

#include <stdexcept>
#include <cstdint>
#include <ctime>
#include <optional>

#include "GameStatesArray.h"
#include "GameState.h"
#include "kayles_types.h"

class ServerService {
	private:
		ClientMessage* message;
		
		std::optional<uint32_t> openGameID;
		
		GameStatesArray games;

	public:

		ServerService(time_t, uint8_t*, uint8_t);
		~ServerService();

		/*
		 * perform_requests() - chooses which action to perform 
		 * based on the parsed ClientMessage
		 */
		void perform_requests();

		void set_message(ClientMessage*);

};

#endif // SERVERSERVICE_H
