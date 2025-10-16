#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include "../GameEngine/GameEngine.h"
#include "../CommandProcessing/CommandProcessing.h"

using namespace std;
using namespace WarzoneCommand;
using namespace WarzoneEngine;

/**
 * @file CommandProcessingDriver.h
 * @brief Contains driver function declarations for testing the CommandProcessor and FileCommandProcessorAdapter classes.
 * 
 * This header declares all driver entry points used to test:
 * - CommandProcessor (console input)
 * - FileCommandProcessorAdapter (file input)
 * - CommandProcessor file generation and integration
 */

/**
 * @brief Driver function to test the CommandProcessor class via console input.
 * 
 * Allows manual interactive testing of command entry and validation.
 */
void testCommandProcessorCLI();

/**
 * @brief Driver function to test the FileCommandProcessorAdapter class.
 * 
 * Executes multiple game simulations using pre-generated text files.
 * 
 * @param fileList Vector of filenames containing sequential game commands.
 */
void testFileCommandProcessorAdapter(const vector<string>& fileList);

/**
 * @brief Driver function to create test files for the CommandProcessor and FileCommandProcessorAdapter.
 * 
 * Automatically generates 8 test input files in the directory:
 * "../CommandProcessor/FileCommandProcessorAdapterInputs/"
 * 
 * Each file represents a unique Warzone simulation scenario as per A2 requirements.
 */
void testFileCommandProcessorAdapterDriver();

/**
 * @brief Master driver that executes all CommandProcessor tests.
 * 
 * Runs:
 * 1. testCommandProcessorCLI() – console-based simulation.
 * 2. testFileCommandProcessorAdapterDriver() – builds and executes file-based tests.
 */
void testCommandProcessor();
