#pragma once

#include "GameEngine.h"
#include <iostream>

using namespace std;
using namespace WarzoneEngine; 
/**
 * @brief Test function for the GameEngine state machine.
 * Allows the user to type commands (loadmap, validatemap, addplayer, assigncountries, 
 * assignreinforcement, issueorder, execorder, win, end) and transitions states accordingly.
 */
void testGameStates();
