#pragma once

#include "GameEngine.h"
#include <iostream>

using namespace std;
using namespace WarzoneEngine;

/**
 * @brief Automated tournament driver for Part 2 tests.
 *
 * Provides a single entry point `testTournament()` which constructs
 * sample maps and strategies and invokes `GameEngine::runTournament(...)`.
 */
void testTournament();
