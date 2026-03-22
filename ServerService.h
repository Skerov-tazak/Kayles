#ifndef SERVERSERVICE_H
#define SERVERSERVICE_H

#include <cstdint>
#include <ctime>
#include "kayles_types.h"
#include "GameState.h"
#include <optional>
#include "GameStatesArray.h"

class serverService {
	private:
		ClientMessage latest_message;
		
		std::optional<GameState> openGame;
		
		GameStatesArray games;

	public:

		serverService(time_t);

		/*
		 * call_functions() - chooses which function to call 
		 * based on the parsed ClientMessage
		 */
		void call_functions(ClientMessage);

		/*
		 * Will keep this game alive on request on player_id
		 */
		void keep_alive(int32_t game_id, int32_t player_id);
		
		/*
		 * Moves one pawn at described location
		 */
		void move_one_pawn(int32_t game_id, int32_t player_id, int8_t pawn);

		/*
		 * Moves two pawns at described location
		 */
		void move_two_pawns(int32_t game_id, int32_t player_id, int8_t pawn);

		/*
		 * Finishes a game and moves it to dead qeue
		 */
		void finish_game(int32_t game_id, int32_t loser_id);

		/*
		 * create_game() - creates a GameState object and assigns it an ID
		 */
		void create_game();

		/*
		 * delete_game() - removes the game object from the table 
		 * */
		void delete_game();

		/*
		 * 
		 * */

};

#endif // SERVERSERVICE_H
