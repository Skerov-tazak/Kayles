#include "GameState.h"

GameState::GameState(int32_t player_a_id, int32_t game_id)
	: game_id(game_id), player_a_id(player_a_id), player_b_id(0), status(WAITING_FOR_OPONENT), max_pawn(0), pawn_row(nullptr) {
	// TODO: Finish Game Constructor
	last_activity = time(NULL);
}

int32_t GameState::get_game_id() const {
	return game_id;
}

int32_t GameState::get_player_a_id() const {
	return player_a_id;
}

int32_t GameState::get_player_b_id() const {
	return player_b_id;
}

Status GameState::get_status() const {
	return status;
}

time_t GameState::get_last_activity() const {
	return last_activity;
}

int8_t GameState::get_max_pawn() const {
	return max_pawn;
}

int8_t* GameState::get_pawn_row() const {
	return pawn_row;
}

void GameState::remove_one_pawn(int32_t pawn_number) {
	// TODO: Implement remove_one_pawn
}

void GameState::remove_two_pawns(int32_t pawn_number) {
	// TODO: Implement remove_two_pawns
}

void GameState::refresh_activity() {
	last_activity = time(NULL);
}
