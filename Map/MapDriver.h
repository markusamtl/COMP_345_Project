#pragma once

#include <iostream>
#include <filesystem>
#include <vector>
#include <string>
#include "Map.h"
#include "../Player/Player.h"

using namespace WarzonePlayer;

/**
 * @brief Free function to test loading and validating ALL .map files from the "test_maps" directory and its subfolders.
 */
void testLoadMaps();
