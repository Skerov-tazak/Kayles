#ifndef MESSAGEMAPPER_H
#define MESSAGEMAPPER_H

#include <cstdint>
#include <ctime>
#include <netinet/in.h>
#include<cstring>

#include "kayles_types.h"
#include "GameState.h"

class MessageMapper {

public:
		static ClientMessage parse_client_message(char* msg);
		
		static void encode_client_message(ClientMessage, char* buffer);

		static GameState parse_gamestate_message(char* msg);

		static void encode_gamestate_message(GameState&, char* buffer);

};
#endif // MESSAGEMAPPER_H
