#include "GameState.h"
#include "kayles_types.h"
#include <sstream>

GameState::GameState(uint32_t player_a_id, uint32_t game_id, uint8_t* pawn_template, uint8_t max_pawn)
	: game_id(game_id), player_a_id(player_a_id), player_b_id(0), status(WAITING_FOR_OPPONENT), max_pawn(max_pawn) 
{
	last_activity = std::time(0);
	size_t bitmap_size = max_pawn / 8 + 1;
	pawn_row = new uint8_t[bitmap_size]; 
	std::copy(pawn_template, pawn_template + bitmap_size, pawn_row);
}

GameState::~GameState()
{
	delete[] pawn_row;
}

GameState::GameState(const GameState& other) 
	: game_id(other.game_id), player_a_id(other.player_a_id), player_b_id(other.player_b_id), 
	  status(other.status), max_pawn(other.max_pawn), last_activity(other.last_activity)
{
	size_t bitmap_size = max_pawn / 8 + 1;
	pawn_row = new uint8_t[bitmap_size];
	std::copy(other.pawn_row, other.pawn_row + bitmap_size, pawn_row);
}

GameState& GameState::operator=(const GameState& other) {
	if (this != &other) {
		delete[] pawn_row;
		game_id = other.game_id;
		player_a_id = other.player_a_id;
		player_b_id = other.player_b_id;
		status = other.status;
		max_pawn = other.max_pawn;
		last_activity = other.last_activity;
		size_t bitmap_size = max_pawn / 8 + 1;
		pawn_row = new uint8_t[bitmap_size];
		std::copy(other.pawn_row, other.pawn_row + bitmap_size, pawn_row);
	}
	return *this;
}

GameState::GameState(GameState&& other) noexcept
	: game_id(other.game_id), player_a_id(other.player_a_id), player_b_id(other.player_b_id),
	  status(other.status), max_pawn(other.max_pawn), pawn_row(other.pawn_row), last_activity(other.last_activity)
{
	other.pawn_row = nullptr;
}

GameState& GameState::operator=(GameState&& other) noexcept {
	if (this != &other) {
		delete[] pawn_row;
		game_id = other.game_id;
		player_a_id = other.player_a_id;
		player_b_id = other.player_b_id;
		status = other.status;
		max_pawn = other.max_pawn;
		pawn_row = other.pawn_row;
		last_activity = other.last_activity;
		other.pawn_row = nullptr;
	}
	return *this;
}

uint32_t GameState::get_game_id() const {
	return game_id;
}

uint32_t GameState::get_player_a_id() const {
	return player_a_id;
}

uint32_t GameState::get_player_b_id() const {
	return player_b_id;
}

Status GameState::get_status() const {
	return status;
}

time_t GameState::get_last_activity() const {
	return last_activity;
}

uint8_t GameState::get_max_pawn() const {
	return max_pawn;
}

uint8_t* GameState::get_pawn_row() const {
	return pawn_row;
}

void GameState::set_status(Status new_status){
	refresh_activity();
	status = new_status;
}

void GameState::set_player_b(uint32_t player_id) {
	player_b_id = player_id;
}

void GameState::remove_one_pawn(uint32_t pawn_number) {
	if(pawn_number > max_pawn){
		throw ILLEGAL_MOVE;
	}

	size_t byte_index = pawn_number / 8;
	uint8_t bit_offset = 7 - (pawn_number % 8); // MSB-first

	uint8_t mask = 1U << bit_offset;

	if ((pawn_row[byte_index] & mask) == 0) {
		throw ILLEGAL_MOVE;
	}

	pawn_row[byte_index] &= ~mask;
}

bool GameState::boardIsEmpty(){
	size_t bitmap_size = max_pawn / 8 + 1;
	for(size_t i = 0; i < bitmap_size; i++){
		if(pawn_row[i] != 0) {
			// Mask out redundant bits if it's the last byte
			if (i == bitmap_size - 1) {
				uint8_t valid_bits = (max_pawn % 8) + 1;
				uint8_t mask = 0;
				for (int b = 0; b < valid_bits; ++b) mask |= (1 << (7 - b));
				if ((pawn_row[i] & mask) != 0) return false;
			} else {
				return false;
			}
		}
	}
	return true;
}

void GameState::remove_two_pawns(uint32_t pawn_number) {
	if(pawn_number >= max_pawn){
		throw ILLEGAL_MOVE;
	}

	size_t byte_index_1 = pawn_number / 8;
	uint8_t bit_offset_1 = 7 - (pawn_number % 8);
	uint8_t mask_1 = 1U << bit_offset_1;

	size_t pawn_number_2 = pawn_number + 1;
	size_t byte_index_2 = pawn_number_2 / 8;
	uint8_t bit_offset_2 = 7 - (pawn_number_2 % 8);
	uint8_t mask_2 = 1U << bit_offset_2;

	bool bit1_is_set = (pawn_row[byte_index_1] & mask_1) != 0;
	bool bit2_is_set = (pawn_row[byte_index_2] & mask_2) != 0;

	if (!bit1_is_set || !bit2_is_set) {
		throw ILLEGAL_MOVE;
	}

	pawn_row[byte_index_1] &= ~mask_1;
	pawn_row[byte_index_2] &= ~mask_2;
}

void GameState::refresh_activity() {
	last_activity = std::time(0);
}

std::string GameState::to_string() const {
	std::ostringstream oss;
	oss << "GameState[id=" << game_id 
	    << ", playerA=" << player_a_id 
	    << ", playerB=" << player_b_id 
	    << ", status=" << (int)status 
	    << ", maxPawn=" << (int)max_pawn 
	    << ", board=";
	
	for (int i = 0; i <= max_pawn; ++i) {
		int byte_idx = i / 8;
		int bit_idx = 7 - (i % 8);
		if (pawn_row[byte_idx] & (1 << bit_idx)) {
			oss << "I";
		} else {
			oss << ".";
		}
	}
	oss << "]";
	return oss.str();
}
