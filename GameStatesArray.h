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
		using Slot = std::variant<int32_t, GameState>;
		
		const int32_t INITIAL = 100;

		std::vector<Slot> slots;
		
		time_t timeout;

		int8_t* pawn_template;

		int8_t max_pawn;

		int32_t head_free_index;
		
		// Private delete
		void deleteElem(int32_t);

		void increase_size();

		void cleanse_timeouted_games();

	public:

		GameStatesArray(time_t, int8_t*, int8_t);
	
		~GameStatesArray();

		// Inserts a new game state into the pool and returns its index
		int32_t insertNewElem(int32_t);

		// Retrieves a game state by ID
		GameState* get_game_state(int32_t);
};

#endif // GAMESTATESARRAY_H
