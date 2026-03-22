#include "GameState.h"

GameState::GameState(int32_t player_a_id, int32_t game_id, int8_t* pawn_template, int8_t max_pawn)
	: game_id(game_id), player_a_id(player_a_id), player_b_id(0), status(WAITING_FOR_OPONENT), max_pawn(max_pawn) 
{

	last_activity = std::time(0);
	pawn_row = new int8_t[max_pawn]; 
	std::copy(pawn_template, pawn_template + max_pawn, pawn_row);
}

GameState::~GameState()
{
	delete pawn_row;
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

void GameState::set_status(Status new_status){
	status = new_status;
}

void GameState::remove_one_pawn(int32_t pawn_number) {

	if(pawn_number > max_pawn){
		throw std::invalid_argument("One or more pawns out of range!");
	}

	size_t byte_index = pawn_number / 8;
	uint8_t bit_offset = pawn_number % 8;

	uint8_t mask = 1U << bit_offset;

	if ((pawn_row[byte_index] & mask) == 0) {
		throw std::invalid_argument("Pawn bit at position " + std::to_string(pawn_number) + " is already 0."
		);
	}

	pawn_row[byte_index] &= ~mask;
}

bool GameState::boardIsEmpty(){
	for(int i = 0; i < max_pawn; i++){
		if(pawn_row[i] != 0)
			return false;
	}
	return true;
}

void GameState::remove_two_pawns(int32_t pawn_number) {

	if(pawn_number >= max_pawn){
		throw std::invalid_argument("One or more pawns out of range!");
	}

	size_t byte_index_1 = pawn_number / 8;
	uint8_t bit_offset_1 = pawn_number % 8;
	uint8_t mask_1 = 1U << bit_offset_1;

	size_t pawn_number_2 = pawn_number + 1;
	size_t byte_index_2 = pawn_number_2 / 8;
	uint8_t bit_offset_2 = pawn_number_2 % 8;
	uint8_t mask_2 = 1U << bit_offset_2;

	bool bit1_is_set = (pawn_row[byte_index_1] & mask_1) != 0;
	bool bit2_is_set = (pawn_row[byte_index_2] & mask_2) != 0;

	if (!bit1_is_set || !bit2_is_set) {
		throw std::invalid_argument(
			"One or both pawn bits starting at " + std::to_string(pawn_number) + " are already 0.");
	}

	pawn_row[byte_index_1] &= ~mask_1;
	pawn_row[byte_index_2] &= ~mask_2;
}

void GameState::refresh_activity() {
	last_activity = std::time(0);
}
