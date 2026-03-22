#ifndef GAMESTATESARRAY_H
#define GAMESTATESARRAY_H

#include <cstdint>
#include <ctime>
#include <optional>
#include <variant>
#include <vector>

#include "GameState.h"

class GameStatesArray {

	private:

		// The Slot can either hold the index of the next free slot or a GameState
		using Slot = std::variant<uint32_t, GameState>;
		
		const uint32_t INITIAL = 100;

		std::vector<Slot> slots;
		
		time_t timeout;

		uint8_t* pawn_template;

		uint8_t max_pawn;

		uint32_t head_free_index;
		
		// Private delete
		void deleteElem(uint32_t);

		void increase_size();

		void cleanse_timeouted_games();

	public:

		GameStatesArray(time_t, uint8_t*, uint8_t);
	
		~GameStatesArray();

		// Inserts a new game state into the pool and returns its index
		uint32_t insertNewElem(uint32_t);

		// Retrieves a game state by ID
		GameState* get_game_state(uint32_t);
};

#endif // GAMESTATESARRAY_H
