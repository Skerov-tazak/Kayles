#include "ServerService.h"
#include "kayles_types.h"
#include <iostream>

ServerService::ServerService(time_t timestamp, uint8_t* pawn_template, uint8_t max_pawn) 
	: message(nullptr), games(timestamp, pawn_template, max_pawn){
	openGameID = std::nullopt;
}
ServerService::~ServerService(){
}

void ServerService::set_message(ClientMessage* message){
	this->message = message;
}

GameState* ServerService::get_relevant_gamestate(){
	if (!message) return nullptr;
	return games.get_game_state(message->game_id);
}

void ServerService::perform_requests() {
	if (!message) return;
	uint32_t player = message->player_id;
	uint32_t game_id = message->game_id;
	uint32_t pawn = message->pawn;

	std::cout << "Performing: " << message->to_string() << "\n";
	std::fflush(stdout);
	// Improper message with player_id == 0
	if(player == 0)
		throw INVALID_PLAYER;

	// Check if this game exists and if the players are playing it
	GameState* gamestate = nullptr;
	if (message->message_type != MSG_JOIN) {
		gamestate = games.get_game_state(game_id);
		if (gamestate == nullptr)
			throw INVALID_GAME_ID;
		if (gamestate->get_player_a_id() != player && gamestate->get_player_b_id() != player)
			throw INVALID_PLAYER;
	}

	switch (message->message_type) {
		case MSG_JOIN:{
			if(openGameID.has_value()){
				uint32_t id = openGameID.value();
				GameState* open_gamestate = games.get_game_state(id);
				// games.get_game_state calls update_timeout_state, 
				// so if it timed out, status is now WIN_A
				if (open_gamestate && open_gamestate->get_status() == WAITING_FOR_OPPONENT) {
					open_gamestate->set_player_b(message->player_id);
					open_gamestate->set_status(TURN_B);
					message->game_id = openGameID.value();
					openGameID = std::nullopt;
				} else {
					// Timed out or already taken, create new game
					openGameID = games.insertNewElem(player);
					message->game_id = openGameID.value();
				}
			}
			else 
			{
				openGameID = std::make_optional<uint32_t>(games.insertNewElem(player));
				message->game_id = openGameID.value();
				
			}
			break;
		}
		
		case MSG_GIVE_UP:{
			if(gamestate->get_status() == TURN_A && gamestate->get_player_a_id() == player)
				gamestate->set_status(WIN_B);
			else if(gamestate->get_status() == TURN_B && gamestate->get_player_b_id() == player)
				gamestate->set_status(WIN_A);
			else 
				throw ILLEGAL_MOVE;
			break;
		}

		case MSG_MOVE_1:{
			if(gamestate->get_status() == TURN_A && gamestate->get_player_a_id() == player)
			{
				gamestate->remove_one_pawn(pawn);

				if(gamestate->boardIsEmpty())
					gamestate->set_status(WIN_A);
				else 
					gamestate->set_status(TURN_B);
			}
			else if(gamestate->get_status() == TURN_B && gamestate->get_player_b_id() == player)
			{
				gamestate->remove_one_pawn(pawn);

				if(gamestate->boardIsEmpty())
					gamestate->set_status(WIN_B);
				else 
					gamestate->set_status(TURN_A);
			}
			else 
				throw ILLEGAL_MOVE;
			break;
		}
		
		case MSG_MOVE_2:{
			if(gamestate->get_status() == TURN_A && gamestate->get_player_a_id() == player)
			{
				gamestate->remove_two_pawns(pawn);

				if(gamestate->boardIsEmpty())
					gamestate->set_status(WIN_A);
				else 
					gamestate->set_status(TURN_B);
			}
			else if(gamestate->get_status() == TURN_B && gamestate->get_player_b_id() == player)
			{
				gamestate->remove_two_pawns(pawn);

				if(gamestate->boardIsEmpty())
					gamestate->set_status(WIN_B);
				else 
					gamestate->set_status(TURN_A);
			}
			else 
				throw ILLEGAL_MOVE;
			break;
		}
		
		case MSG_KEEP_ALIVE:{
			gamestate->refresh_activity();
			break;
		}
		
		default:{
			throw UKNOWN_MESSAGE_TYPE;
		}
	}
}
