#include "CommandProcessing.h"
#include "../GameEngine/GameEngine.h"
#include <ctime>
#include <filesystem>
#include <iomanip>

using namespace std;
using namespace WarzoneCommand;
using namespace WarzoneEngine;
namespace fs = std::filesystem;

/**
 * @brief Driver function to test the CommandProcessor class, by command line.
 * 
 * This version uses ONLY console input for commands, and interacts directly
 * with the GameEngine instance. All game events and outputs are logged to
 * ../CommandProcessing/Games/ with timestamped filenames.
 * 
 * Each phase dynamically shows the user which commands are valid to enter.
 * It also includes recovery logic — if map loading or validation fails, the
 * engine resets to Start.
 */
void testCommandProcessorCLI();