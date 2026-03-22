#ifndef GAMESTATE_H
#define GAMESTATE_H

#include <cstdint>
#include <ctime>
#include <string>
#include <stdexcept>
#include <algorithm>

#include "kayles_types.h"

class GameState {

private:
	int32_t game_id;
	int32_t player_a_id;
	int32_t player_b_id;
	Status status;
	int8_t max_pawn;
	int8_t* pawn_row;
	time_t last_activity;

public:
	GameState(int32_t player_a_id, int32_t game_id, int8_t* pawn_template, int8_t max_pawn);
	~GameState();

	int32_t get_game_id() const;
	int32_t get_player_a_id() const;
	int32_t get_player_b_id() const;
	Status get_status() const;
	time_t get_last_activity() const;
	int8_t get_max_pawn() const;
	int8_t* get_pawn_row() const;

	void set_status(Status);
	void set_player_b(int32_t);

	bool boardIsEmpty();

	void remove_one_pawn(int32_t pawn_number);

	void remove_two_pawns(int32_t pawn_number);

	void refresh_activity();

};

#endif
