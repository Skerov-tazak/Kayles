#ifndef KAYLES_TYPES_H
#define KAYLES_TYPES_H

#include <cstdint>
#include <ctime>

enum Status : uint8_t{
	WAITING_FOR_OPPONENT = 0,
	TURN_A = 1,
	TURN_B = 2,
	WIN_A = 3,
	WIN_B = 4
};

enum MessageType : uint8_t{
	MSG_JOIN = 0,
	MSG_MOVE_1 = 1,
	MSG_MOVE_2 = 2,
	MSG_KEEP_ALIVE = 3,
	MSG_GIVE_UP = 4
};

enum ErrorType : uint8_t{
	UKNOWN_MESSAGE_TYPE = 0,
	INVALID_PLAYER = 1,
	INVALID_GAME_ID = 5,
	INVALID_PAWN = 11
};

struct ClientMessage {
	MessageType message_type;
	uint32_t player_id;
	uint32_t game_id;
	uint8_t pawn;
};

#endif // KAYLES_TYPES_H
