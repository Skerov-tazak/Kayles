#ifndef MESSAGEMAPPER_H
#define MESSAGEMAPPER_H

#include <cstdint>
#include <ctime>

#include "kayles_types.h"
#include "GameState.h"

class messageMapper {

public:
		static ClientMessage parse_client_message(char* msg);
		
		static char* encode_client_message(ClientMessage);

		static GameState parse_gamestate_message(char* msg);

		static char* encode_gamestate_message(GameState);

};
#endif // MESSAGEMAPPER_H
