#ifndef MESSAGEMAPPER_H
#define MESSAGEMAPPER_H

#include <cstdint>
#include <ctime>
#include "kayles_types.h"
#include "GameState.h"

class messageMapper {

public:
		ClientMessage parse_client_message(char* msg);
		
		char* encode_client_message(ClientMessage);

		GameState parse_gamestate_message(char* msg);

		char* encode_gamestate_message(GameState);

};
#endif // MESSAGEMAPPER_H
