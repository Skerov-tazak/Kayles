#include "ServerService.h"
#include "kayles_types.h"

ServerService::ServerService(time_t timestamp, uint8_t* pawn_template, uint8_t max_pawn) 
	: games(timestamp, pawn_template, max_pawn){
	openGameID = std::nullopt;
}
ServerService::~ServerService(){
	delete message;
}

void ServerService::set_message(ClientMessage* message){
	delete this->message;
	this->message = message;
}

GameState* ServerService::get_relevant_gamestate(){
	return games.get_game_state(message->game_id);
}

void ServerService::perform_requests() {
	uint32_t player = message->player_id;
	uint32_t game_id = message->game_id;
	uint32_t pawn = message->pawn;

	switch (message->message_type) {
		case MSG_JOIN:{
			if(openGameID.has_value()){
				uint32_t id = openGameID.value();
				GameState* gamestate = games.get_game_state(id);
				gamestate->set_player_b(message->player_id);
				gamestate->set_status(TURN_B);
				openGameID = std::nullopt;
			}
			else 
			{
				openGameID = std::make_optional<uint32_t>(games.insertNewElem(player));
			}
			break;
		}
		
		case MSG_GIVE_UP:{
			GameState* gamestate = games.get_game_state(game_id);
			if(gamestate == nullptr)
				throw INVALID_GAME_ID;
			if(gamestate->get_player_a_id() == player && gamestate->get_status() == TURN_A)
			{
				gamestate->set_status(WIN_B);
			}
			else if(gamestate->get_player_b_id() == player && gamestate->get_status() == TURN_B)
			{
				gamestate->set_status(WIN_A);
			}
			else 
			{
				throw INVALID_PLAYER;
			}
			break;
		}

		case MSG_MOVE_1:{
			GameState* gamestate = games.get_game_state(game_id);
			if(gamestate == nullptr)
				throw INVALID_GAME_ID;

			if(gamestate->get_player_a_id() == player && gamestate->get_status() == TURN_A)
			{
				gamestate->remove_one_pawn(pawn);

				if(gamestate->boardIsEmpty())
					gamestate->set_status(WIN_A);
				else 
					gamestate->set_status(TURN_B);
			}
			else if(gamestate->get_player_b_id() == player && gamestate->get_status() == TURN_B)
			{
				gamestate->remove_one_pawn(pawn);

				if(gamestate->boardIsEmpty())
					gamestate->set_status(WIN_B);
				else 
					gamestate->set_status(TURN_A);
			}
			else 
			{
				throw INVALID_PLAYER;
			}
			break;
		}
		
		case MSG_MOVE_2:{
			GameState* gamestate = games.get_game_state(game_id);
			if(gamestate == nullptr)
				throw INVALID_GAME_ID;

			if(gamestate->get_player_a_id() == player && gamestate->get_status() == TURN_A)
			{
				gamestate->remove_two_pawns(pawn);

				if(gamestate->boardIsEmpty())
					gamestate->set_status(WIN_A);
				else 
					gamestate->set_status(TURN_B);
			}
			else if(gamestate->get_player_b_id() == player && gamestate->get_status() == TURN_B)
			{
				gamestate->remove_two_pawns(pawn);

				if(gamestate->boardIsEmpty())
					gamestate->set_status(WIN_B);
				else 
					gamestate->set_status(TURN_A);
			}
			else 
			{
				throw INVALID_PLAYER;
			}
			break;
		}
		
		case MSG_KEEP_ALIVE:{
			GameState* gamestate = games.get_game_state(game_id);
			if(gamestate == nullptr)
				throw INVALID_GAME_ID;

			if(gamestate->get_player_a_id() == player && gamestate->get_status() == TURN_A ||
					gamestate->get_player_b_id() == player && gamestate->get_status() == TURN_B) {
				
				gamestate->refresh_activity();
			}
			else 
			{
				throw INVALID_PLAYER;
			}
			break;
		}
		
		default:{
			throw UKNOWN_MESSAGE_TYPE;
		}
	}
}
