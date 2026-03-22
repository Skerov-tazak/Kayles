#ifndef GAMESTATESARRAY_H
#define GAMESTATESARRAY_H

#include <cstdint>
#include <ctime>
#include <unordered_map>
#include <list>
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

		int32_t head_free_index;
		
		std::unordered_map<int32_t, int32_t> id_to_index;
		
		struct DeadNode 
		{
			int32_t id;
			time_t timestamp;
		};

		class DeadQueue 
		{
			private:
				std::list<DeadNode> dll;

				// Maps indexes to linked list nodes
				std::unordered_map<int32_t, std::list<DeadNode>::iterator> hash_map;
				
				// Timeout constant
				time_t timeout;

			public:

				DeadQueue(time_t);
				
				void put(int32_t id, time_t timestamp);
				
				void refresh(int32_t id, time_t timestamp);
				
				std::optional<int32_t> pop();
		};


		DeadQueue dead_queue;
		// Private delete which will only be called legally by DeadQueue
		void deleteElem(int32_t id);

		void increase_size();

		void cleanse_timeouted_games();

	public:

		GameStatesArray(time_t);

		// Moves the game to the DeadQueue
		void killGame(int32_t id, time_t timestamp);

		// Inserts a new game state into the pool
		void insertNewElem(int32_t game_id, int32_t player_a_id);

		// Retrieves a game state by ID
		GameState* get_game_state(int32_t game_id);
};

#endif // GAMESTATESARRAY_H
