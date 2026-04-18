#include "GameStatesArray.h"
#include "GameState.h"
#include "kayles_types.h"
#include <cstdint>

// GameStatesArray implementation
GameStatesArray::GameStatesArray(time_t timeout, uint8_t* pawn_template, uint8_t max_pawn) 
	: timeout(timeout), pawn_template(pawn_template), max_pawn(max_pawn){

	// Creates the initial free list
	slots.resize(INITIAL);

	for (size_t i = 0; i < INITIAL - 1; ++i) {
		slots[i] = static_cast<uint32_t>(i + 1);
	}

	slots[INITIAL - 1] = static_cast<uint32_t>(-1);
	head_free_index = 0;
}

GameStatesArray::~GameStatesArray(){
	delete[] pawn_template;
}

void GameStatesArray::increase_size(){

	uint32_t first_new = slots.size();
	uint32_t old_head = head_free_index;
	slots.resize(slots.size() * 2);
	
	for(size_t i = first_new; i < slots.size() - 1; i++){
		slots[i] = static_cast<uint32_t>(i + 1);
	}

	slots[slots.size() - 1] = old_head;
	head_free_index = first_new;
}

void GameStatesArray::deleteElem(uint32_t id) {
		slots[id] = head_free_index;
		head_free_index = id;
}

void GameStatesArray::update_timeout_state(GameState* gamestate)
{
	if(std::time(0) - gamestate->get_last_activity() > timeout){
		Status state = gamestate->get_status();
		if(state == TURN_B || state == WAITING_FOR_OPPONENT){
			gamestate->set_status(WIN_A);
		}
		else if(state == TURN_A) 
		{
			gamestate->set_status(WIN_B);
		}
	}
}

uint32_t GameStatesArray::cleanse_timeouted_games()
{
	uint32_t cleaned_number = 0;
	// Iterate over all elements and cleanse those that have timed out
	for(auto& elem : slots){
		if(GameState* gamestate = std::get_if<GameState>(&elem)){
			if(std::time(0) - gamestate->get_last_activity() > timeout){
				// Only cleanse timeouted games that have ended
				// Or those that have ended and their timer has run out afterwards too
				if(gamestate->get_status() == WIN_A || gamestate->get_status() == WIN_B
						|| std::time(0) - gamestate->get_last_activity() > timeout * 2){
					deleteElem(gamestate->get_game_id());
					cleaned_number++;
				}
				else 
				{
					update_timeout_state(gamestate);
				}
			}
		}
	}

	return cleaned_number;
}

uint32_t GameStatesArray::insertNewElem(uint32_t player_a_id) 
{
	// If Pool is full then try to make space
	// Head free index will be -1 if and only if we have filled the entire array
	if (head_free_index == (uint32_t)-1) {
		
		// We allocate more space if the cleanup didn't help a lot
		if(cleanse_timeouted_games() < slots.size()/4)
			increase_size();
	}

	uint32_t new_index = head_free_index;

	// Update head_free_index to the next available slot in the chain
	head_free_index = std::get<uint32_t>(slots[new_index]);

	// Overwrite with the new GameState
	slots[new_index] = GameState(player_a_id, new_index, pawn_template, max_pawn);
	return new_index;
}

GameState* GameStatesArray::get_game_state(uint32_t game_id) 
{
	if(game_id >= slots.size())
		return nullptr;

	if(GameState* gamestate = std::get_if<GameState>(&slots[game_id])){
		// We update the timeout state to set to WIN_A or WIN_B 
		// before passing it to ServerService
		update_timeout_state(gamestate);
		return gamestate;
	}
	else 
		return nullptr;
}

