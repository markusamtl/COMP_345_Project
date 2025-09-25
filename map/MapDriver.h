#pragma once
#include <string>

/**
 * @brief Free function to test loading of random map files
 * @param directory Path to the folder containing map subfolders
 * @param numOfMapsToLoad Number of random maps to select and test
 */
void testLoadMaps(const std::string& directory, int numOfMapsToLoad);
