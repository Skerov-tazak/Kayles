#include "ServerService.h"

ServerService::ServerService(time_t timestamp, int8_t* pawn_template, int8_t max_pawn) 
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

void ServerService::perform_requests() {
	int32_t player = message->player_id;
	int32_t game_id = message->game_id;
	int32_t pawn = message->pawn;

	switch (message->message_type) {
		case MSG_JOIN:{
			if(openGameID.has_value()){
				int32_t id = openGameID.value();
				GameState* gamestate = games.get_game_state(id);
				gamestate->set_player_b(message->player_id);
				gamestate->set_status(TURN_B);
				openGameID = std::nullopt;
			}
			else 
			{
				openGameID = std::make_optional<int32_t>(games.insertNewElem(player));
			}
			break;
		}
		
		case MSG_GIVE_UP:{
			GameState* gamestate = games.get_game_state(game_id);
			if(gamestate == nullptr)
				throw std::invalid_argument("This game does not exist");
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
				throw std::invalid_argument("This player can't give up! He is not taking part in this \
						game or this is not his turn!");
			}
			break;
		}

		case MSG_MOVE_1:{
			GameState* gamestate = games.get_game_state(game_id);
			if(gamestate == nullptr)
				throw std::invalid_argument("This game does not exist");

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
				throw std::invalid_argument("This player can't give up! He is not taking part in this \
						game or this is not his turn!");
			}
			break;
		}
		
		case MSG_MOVE_2:{
			GameState* gamestate = games.get_game_state(game_id);
			if(gamestate == nullptr)
				throw std::invalid_argument("This game does not exist");

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
				throw std::invalid_argument("This player can't give up! He is not taking part in this \
						game or this is not his turn!");
			}
			break;
		}
		
		case MSG_KEEP_ALIVE:{
			GameState* gamestate = games.get_game_state(game_id);
			if(gamestate == nullptr)
				throw std::invalid_argument("This game does not exist");

			if(gamestate->get_player_a_id() == player && gamestate->get_status() == TURN_A)
			{
				gamestate->refresh_activity();
			}
			else if(gamestate->get_player_a_id() == player && gamestate->get_status() == TURN_A)
			{
				gamestate->refresh_activity();
			}
			else 
			{
				throw std::invalid_argument("This player can't give up! He is not taking part in this \
						game or this is not his turn!");
			}
			break;
		}
		
		default:{
			throw std::invalid_argument("Unrecognised message code");
		}
	}
}
