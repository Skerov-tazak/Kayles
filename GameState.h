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
	uint32_t game_id;
	uint32_t player_a_id;
	uint32_t player_b_id;
	Status status;
	uint8_t max_pawn;
	uint8_t* pawn_row;
	time_t last_activity;

public:
	GameState(uint32_t player_a_id, uint32_t game_id, uint8_t* pawn_template, uint8_t max_pawn);
	~GameState();

	// Rule of Five
	GameState(const GameState& other);
	GameState& operator=(const GameState& other);
	GameState(GameState&& other) noexcept;
	GameState& operator=(GameState&& other) noexcept;

	uint32_t get_game_id() const;
	uint32_t get_player_a_id() const;
	uint32_t get_player_b_id() const;
	Status get_status() const;
	time_t get_last_activity() const;
	uint8_t get_max_pawn() const;
	uint8_t* get_pawn_row() const;

	void set_status(Status);
	void set_player_b(uint32_t);

	bool boardIsEmpty();

	void remove_one_pawn(uint32_t pawn_number);

	void remove_two_pawns(uint32_t pawn_number);

	void refresh_activity();

	std::string to_string() const;

};

#endif
