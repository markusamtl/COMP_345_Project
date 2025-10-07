/**
 * @file MainDriver.cpp
 * @brief Main entry point for the Warzone game system testing.
 * 
 * This driver sequentially calls all five component test functions:
 *   1. testMap()         - Validates map loading, parsing, and validation.
 *   2. testPlayer()      - Tests player creation, territory assignment, and order issuing.
 *   3. testOrdersList()  - Tests order creation, validation, and execution logic.
 *   4. testCards()       - Tests deck/hand/card draw and play mechanics.
 *   5. testGameEngine()  - Simulates end-to-end game setup and state transitions.
 *
 * Each module is tested independently but in order, ensuring that dependencies
 * (e.g., Map -> Player -> Orders -> Cards -> Engine) are properly demonstrated.
 */

#include <iostream>
#include "Map/MapDriver.h"
#include "Player/PlayerDriver.h"
#include "Order/OrderDriver.h"
#include "Card/CardDriver.h"
#include "GameEngine/GameEngineDriver.h"

using namespace std;

int main() {
    
    cout << "=============================================\n";
    cout << "        COMP 345 - WARZONE MAIN DRIVER       \n";
    cout << "=============================================\n";

    cout << "\n[1] Testing Map Module...\n";
    testLoadMaps();

    cout << "\n[2] Testing Player Module...\n";
    testPlayer();

    cout << "\n[3] Testing Orders List Module...\n";
    testOrderList();

    cout << "\n[4] Testing Cards Module...\n";
    testCards();

    cout << "\n[5] Testing Game Engine Module...\n";
    testGameStates();

    cout << "\n[6] Testing Game Simulation...\n";
    simulateRealGame();

    cout << "\n=============================================\n";
    cout << " All modules tested successfully. Exiting...\n";
    cout << "=============================================\n";

    return 0;

}
