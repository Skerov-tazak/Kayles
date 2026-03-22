#include "GameStatesArray.h"
#include "GameState.h"

// GameStatesArray implementation
GameStatesArray::GameStatesArray(time_t timeout, int8_t* pawn_template, int8_t max_pawn) 
	: timeout(timeout), pawn_template(pawn_template), max_pawn(max_pawn){

	// Creates the initial free list
	slots.resize(INITIAL);

	for (size_t i = 0; i < INITIAL - 1; ++i) {
		slots[i] = static_cast<int32_t>(i + 1);
	}

	slots[INITIAL - 1] = static_cast<int32_t>(-1);
	head_free_index = 0;
}

GameStatesArray::~GameStatesArray(){
	delete pawn_template;
	for(auto x : slots){
		if(GameState* gamestate = std::get_if<GameState>(&x))
			delete gamestate;
	}
}

void GameStatesArray::increase_size(){

	int32_t first_new = slots.size();
	slots.resize(slots.size() * 2);
	
	for(size_t i = first_new; i < slots.size() - 1; i++){
		slots[i] = static_cast<int32_t>(i + 1);
	}

	slots[slots.size() - 1] = static_cast<int32_t>(-1);
	head_free_index = first_new;
}

void GameStatesArray::deleteElem(int32_t id) {
		
		GameState* gamestate = &std::get<GameState>(slots[id]);
		delete gamestate;
		slots[id] = head_free_index;
		head_free_index = id;
}

void GameStatesArray::cleanse_timeouted_games()
{
	// Iterate over all elements and cleanse those that have timed out
	for(auto elem : slots){
		GameState* gamestate = &std::get<GameState>(elem);
		if(std::time(0) - gamestate->get_last_activity() > timeout)
			deleteElem(gamestate->get_game_id());
	}

}

int32_t GameStatesArray::insertNewElem(int32_t player_a_id) 
{
	// If Pool is full then try to make space
	if (head_free_index == -1) {
		cleanse_timeouted_games();
		
		// If it failed then make it bigger
		if(head_free_index == -1)
			increase_size();
	}

	int32_t new_index = head_free_index;

	// Update head_free_index to the next available slot in the chain
	head_free_index = std::get<int32_t>(slots[new_index]);

	// Overwrite with the new GameState
	slots[new_index] = GameState(player_a_id, new_index, pawn_template, max_pawn);
	return new_index;
}

GameState* GameStatesArray::get_game_state(int32_t game_id) 
{
	if(GameState* gamestate = std::get_if<GameState>(&slots[game_id]))
		return gamestate;
	else 
		return nullptr;
}
