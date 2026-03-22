#include "GameStatesArray.h"
#include "GameState.h"
#include <cstdint>
#include <ctime>
#include <iostream>
#include <optional>

// DeadQueue implementation
GameStatesArray::DeadQueue::DeadQueue(time_t t)
{
	this->timeout = t;
}

void GameStatesArray::DeadQueue::put(int32_t id, time_t timestamp) 
{
	dll.push_back({id, timestamp});
	hash_map[id] = std::prev(dll.end());
}

void GameStatesArray::DeadQueue::refresh(int32_t id, time_t timestamp) 
{
	auto it = hash_map.find(id);
	if (it != hash_map.end()) 
	{
		it->second->timestamp = timestamp;
		dll.splice(dll.end(), dll, it->second);
	}
}

std::optional<int32_t> GameStatesArray::DeadQueue::pop() 
{
	// Return nullopt if the queue is empty
	if (dll.empty()) return std::nullopt;

	time_t now = std::time(nullptr);
	if (now - dll.front().timestamp >= timeout) 
	{
		int32_t id = dll.front().id;
		hash_map.erase(id);
		dll.pop_front();
		return id;
	}

	// Return nullopt if the oldest element is not older than timeout
	return std::nullopt;
}


// GameStatesArray implementation
GameStatesArray::GameStatesArray(time_t timeout) : timeout(timeout), dead_queue(timeout) 
{
	// Creates the initial free list
	slots.resize(INITIAL);

	for (size_t i = 0; i < INITIAL - 1; ++i) {
		slots[i] = static_cast<int32_t>(i + 1);
	}

	slots[INITIAL - 1] = static_cast<int32_t>(-1);
}

void GameStatesArray::increase_size(){

	int32_t first_new = slots.size();
	slots.resize(slots.size() * 2);
	
	for(int i = first_new; i < slots.size() - 1; i++){
		slots[first_new] = static_cast<int32_t>(i + 1);
	}

	slots[slots.size() - 1] = static_cast<int32_t>(-1);
	head_free_index = first_new;
}

void GameStatesArray::deleteElem(int32_t id) {
	auto it = id_to_index.find(id);
	if (it != id_to_index.end()) {
		int32_t index = it->second;
		// The slot now holds the previous head_free_index (linking to the rest of the free list)
		slots[index] = head_free_index;
		// head_free_index now points to this freshly freed slot
		head_free_index = index;
		// Remove from ID map
		id_to_index.erase(it);
	}
}

void GameStatesArray::killGame(int32_t id, time_t timestamp) {

	// Moving the game to the DeadQueue. 
	// It will be deleted legally later when it pops out of the queue.
	if (id_to_index.find(id) != id_to_index.end()) {
		dead_queue.put(id, timestamp);
	}
}

void GameStatesArray::cleanse_timeouted_games()
{
	// Iterate over all elements and cleanse those that have timed out
	for(auto elem : slots){
		GameState* gamestate = &std::get<GameState>(elem);
		if()
	}

}

void GameStatesArray::insertNewElem(GameState* gamestate) 
{
	// First try the top element from DeadQueue - if 
	// any element is expired in dead queue it will become a
	// free slot now
	std::optional<int32_t> longest_dead = dead_queue.pop();
	
	if(longest_dead.has_value())
	{
		deleteElem(longest_dead.value());
	}

	// If Pool is full then try to make it bigger
	if (head_free_index == -1) {

		cleanse_timeouted_games();
		
		if(head_free_index == -1)
			increase_size();
	}

	int32_t new_index = head_free_index;

	// Update head_free_index to the next available slot in the chain
	// Use std get for variant
	head_free_index = std::get<int32_t>(slots[new_index]);

	// Overwrite with the new GameState
	slots[new_index] = GameState(player_a_id, game_id);
	id_to_index[game_id] = new_index;

}

GameState* GameStatesArray::get_game_state(int32_t game_id) 
{
	auto it = id_to_index.find(game_id);
	if (it != id_to_index.end()) 
	{
		// Return GameState type from variant
		return &std::get<GameState>(slots[it->second]);
	}

	return nullptr;
}
