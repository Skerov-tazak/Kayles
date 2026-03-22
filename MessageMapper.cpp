#include "MessageMapper.h"

ClientMessage MessageMapper::parse_client_message(char* msg){
	
	uint8_t msg_type_raw = static_cast<uint8_t>(msg[0]);
	uint32_t player_id;
	uint32_t game_id;
	uint8_t pawn = static_cast<uint8_t>(msg[9]);

	std::memcpy(&player_id, &msg[1], 4);
	std::memcpy(&game_id, &msg[5], 4);
	
	player_id = ntohl(player_id);
	game_id = ntohl(game_id);

	return ClientMessage({static_cast<MessageType>(msg_type_raw), player_id, game_id, pawn});
}

void MessageMapper::encode_client_message(ClientMessage msg, char* buffer){
	uint8_t msg_type = static_cast<uint8_t>(msg.message_type);
	uint32_t player_id = htonl(msg.player_id);
	uint32_t game_id = htonl(msg.game_id);
	uint8_t pawn = msg.pawn;

	buffer[0] = msg_type;
	std::memcpy(&buffer[1], &player_id, 4);
	std::memcpy(&buffer[5], &game_id, 4);
	buffer[9] = pawn;
}


GameState MessageMapper::parse_gamestate_message(char* msg){
	uint32_t game_id;
	uint32_t player_a_id;
	uint32_t player_b_id;
	
	std::memcpy(&game_id, &msg[0], 4);
	std::memcpy(&player_a_id, &msg[4], 4);
	std::memcpy(&player_b_id, &msg[8], 4);

	game_id = ntohl(game_id);
	player_a_id = ntohl(player_a_id);
	player_b_id = ntohl(player_b_id);

	Status status = static_cast<Status>(msg[12]);
	uint8_t max_pawn = static_cast<uint8_t>(msg[13]);

	size_t bitmap_size = max_pawn / 8 + 1;
	uint8_t* pawn_row = new uint8_t[bitmap_size];
	std::memcpy(pawn_row, &msg[14], bitmap_size);

	GameState state(player_a_id, game_id, pawn_row, max_pawn);
	state.set_player_b(player_b_id);
	state.set_status(status);

	delete[] pawn_row;
	return state;
}

void MessageMapper::encode_gamestate_message(GameState& state, char* buffer){
	uint32_t game_id = htonl(state.get_game_id());
	uint32_t player_a_id = htonl(state.get_player_a_id());
	uint32_t player_b_id = htonl(state.get_player_b_id());
	uint8_t status = static_cast<uint8_t>(state.get_status());
	uint8_t max_pawn = state.get_max_pawn();

	std::memcpy(&buffer[0], &game_id, 4);
	std::memcpy(&buffer[4], &player_a_id, 4);
	std::memcpy(&buffer[8], &player_b_id, 4);
	buffer[12] = status;
	buffer[13] = max_pawn;

	size_t bitmap_size = max_pawn / 8 + 1;
	std::memcpy(&buffer[14], state.get_pawn_row(), bitmap_size);
}
