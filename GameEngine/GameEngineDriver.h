#pragma once

#include "GameEngine.h"
#include <iostream>

using namespace std;
using namespace WarzoneEngine;

/**
 * @brief Test function for the GameEngine state machine.
 * Allows the user to type commands (loadmap, validatemap, addplayer, gamestart,
 * assignreinforcement, issueorder, endissueorders, executeorder, endexecuteorder, win, end)
 * and transitions states accordingly.
 */
void testGameStates();

/**
 * @brief Simulates a full automatic game sequence with multiple players on a predefined map.
 * The user can specify the maximum number of turns to simulate and whether to print detailed outputs.
 * After the simulation ends, the user is prompted to play again or exit.
 */
void simulateRealGame();
