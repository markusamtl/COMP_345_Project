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
