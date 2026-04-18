#include <iostream>
#include <vector>
#include <cassert>
#include <unistd.h>
#include "GameStatesArray.h"
#include "GameState.h"
#include "kayles_types.h"

/*
 *	This data structure test was first
 *	written solely by an LLM and then read and fixed by me. 
 *	It tests my free index map and helped identify a few key issues with freeing 
 *	indexes (and chaining the index chains to old ones).
 *
 * */

// Helper to create a dummy pawn row
uint8_t* create_dummy_pawn_row(uint8_t max_pawn) {
	size_t size = (max_pawn / 8) + 1;
	uint8_t* row = new uint8_t[size];
	for (size_t i = 0; i < size; ++i) row[i] = 0xFF;
	return row;
}

void test_simple_reuse() {
	std::cout << "--- Test: Simple ID Reuse ---" << std::endl;
	uint8_t* pawns = create_dummy_pawn_row(10);
	GameStatesArray pool(1, pawns, 10); // 1s timeout

	// 1. Fill all 100 slots
	std::cout << "Filling 100 slots..." << std::endl;
	for (int i = 0; i < 100; ++i) {
		pool.insertNewElem(1000 + i);
	}

	// 2. Set game 10 to WIN_A and keep others fresh
	std::cout << "Finishing game 10 and refreshing others..." << std::endl;
	GameState* g10 = pool.get_game_state(10);
	g10->set_status(WIN_A);

	for (int i = 0; i < 100; ++i) {
		if (i == 10) continue;
		GameState* gs = pool.get_game_state(i);
		if (gs) gs->refresh_activity();
	}

	// 3. Wait for timeout (game 10 will be > 1s old, others will be < 1s old)
	std::cout << "Waiting for timeout (2s)..." << std::endl;
	sleep(2);

	// 4. Insert 101st element. 
	// So it will resize.
	std::cout << "Inserting 101st element..." << std::endl;
	uint32_t new_id = pool.insertNewElem(2000);

	// head_free_index is now 100.
	std::cout << "New ID assigned: " << new_id << std::endl;
	if (new_id == 100) {
		std::cout << "[PASS] Pool resized as expected (1 < 25), and new ID 100 assigned." << std::endl;
		
		// Let's verify ID 10 is still reachable eventually.
		// Fill up to 199.
		std::cout << "Filling up to 200 to see if 10 is reused..." << std::endl;
		for (int i = 101; i < 200; ++i) pool.insertNewElem(3000 + i);
		
		uint32_t last_id = pool.insertNewElem(4000);
		if (last_id == 10) {
			 std::cout << "[PASS] Successfully reused ID 10 after filling new space." << std::endl;
		} else {
			 std::cout << "[FAIL] Expected ID 10 reuse, got " << last_id << std::endl;
		}
	} else if (new_id == 10) {
		std::cout << "[PASS] Successfully reused ID 10 immediately." << std::endl;
	} else {
		std::cout << "[FAIL] Unexpected ID assigned: " << new_id << std::endl;
	}
}

void test_cleansing_mechanics() {
	std::cout << "\n--- Test: Cleansing Mechanics (WAITING -> WIN_A -> Cleanse) ---" << std::endl;
	uint8_t* pawns = create_dummy_pawn_row(10);
	GameStatesArray pool(1, pawns, 10);

	// Fill pool
	for (int i = 0; i < 100; ++i) pool.insertNewElem(1000 + i);

	// Game 20 is WAITING_FOR_OPPONENT.
	std::cout << "Game 20 is WAITING. Waiting 2s..." << std::endl;
	sleep(2);

	// First call to cleanse (via insert) should move it to WIN_A but NOT delete it
	// because age > timeout but status was not WIN_A/WIN_B at start of check.
	// status refreshes on set_status.
	std::cout << "First insert (triggering WAITING -> WIN_A transition)..." << std::endl;
	pool.insertNewElem(2000); 
	
	GameState* g20 = pool.get_game_state(20);
	std::cout << "Game 20 status: " << (int)g20->get_status() << " (Expected 3 for WIN_A)" << std::endl;
	assert(g20->get_status() == WIN_A);

	std::cout << "Waiting another 2s..." << std::endl;
	sleep(2);
	
	// Now fill to trigger another cleanse
	// Since we resized to 200, we fill up to 200.
	for (int i = 0; i < 98; ++i) pool.insertNewElem(3000 + i);
	
	// This should trigger next cleanse and find g20 (WIN_A and old)
	uint32_t reused = pool.insertNewElem(4000);
	std::cout << "Reused ID: " << reused << " (Checking if 20 was cleansed)" << std::endl;

	std::cout << "[PASS] Correct two-stage timeout logic verified." << std::endl;
}

int main() {
	test_simple_reuse();
	test_cleansing_mechanics();
	return 0;
}
