#include "GameEngineDriver.h"
#include <algorithm>
#include <iostream>
#include <sstream>
#include <chrono>
#include <fstream>

using namespace WarzoneEngine;

/*---------------------------------- Manual Console Driver ----------------------------------*/

void testGameStates() {
    GameEngine engine;

    cout << "=== WARZONE GAME ENGINE CONSOLE ===\n"
         << "Available commands (case-insensitive):\n"
         << "  loadmap <path>            -> Load a map file\n"
         << "  validatemap               -> Validate the currently loaded map\n"
         << "  addplayer <name>          -> Add a player to the game\n"
         << "  gamestart                 -> Assign territories and start the game\n"
         << "  assignreinforcement       -> Compute and assign reinforcements to all players\n"
         << "  issueorder                -> Each player issues orders\n"
         << "  endissueorders            -> End issuing phase, move to execution\n"
         << "  executeorder              -> Execute all orders interleaved by player\n"
         << "  endexecuteorder           -> End execution phase (loops back to reinforcement)\n"
         << "  win                       -> Show winning state (if applicable)\n"
         << "  end                       -> Terminate the engine\n"
         << "\nExample flow:\n"
         << "  loadmap ../Map/test_maps/Brazil/Brazil.map\n"
         << "  validatemap\n"
         << "  addplayer Alice\n"
         << "  addplayer Bob\n"
         << "  gamestart\n"
         << "  assignreinforcement\n"
         << "  issueorder\n"
         << "  endissueorders\n"
         << "  executeorder\n"
         << "  endexecuteorder\n"
         << "  end\n"
         << "====================================\n\n";

    string line;
    while(engine.getState() != EngineState::End && getline(cin, line)) {
        
        if(line.empty()) continue;

        stringstream ss(line);
        
        string cmd, arg;
        ss >> cmd;
        getline(ss, arg);
        
        if(!arg.empty() && arg[0] == ' '){ arg.erase(0, 1); }

        string result = engine.processCommand(cmd, arg, true);
        cout << "[State] " << engine.getState() << " | " << result << "\n";

        if(cmd == "end" || cmd == "END"){ break; }
    }

    cout << "\n=== GAME ENGINE TERMINATED ===\n";

}

//---------------------------------- Startup Phase Test -------------------------------------------

void testStartupPhase() {

    cout << "=============================================\n";
    cout << "             TEST: STARTUP PHASE             \n";
    cout << "=============================================\n\n";

    GameEngine* gameEngine = new GameEngine();

    bool suppressPrints = false;
    char userChoice;

    cout << "Would you like to suppress console output during the startup phase? (y/n): ";

    while(true) {

        if(!(cin >> userChoice)) {
            
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid input. Please enter 'y' or 'n': ";
            continue;

        }

        userChoice = tolower(userChoice);
        if(userChoice == 'y') {

            suppressPrints = true;
            break;
        
        } else if (userChoice == 'n') {
            
            suppressPrints = false;
            break;

        } else { cout << "Invalid selection. Please enter 'y' or 'n': "; }
    
    }

    cin.ignore(numeric_limits<streamsize>::max(), '\n'); // clear newline

    cout << "\n[Driver] Starting startup phase..." << endl;
    gameEngine -> startupPhase(suppressPrints);

    delete gameEngine;

    cout << "\n=============================================\n";
    cout << "           END OF STARTUP PHASE TEST           \n";
    cout << "=============================================\n\n";

}

//---------------------------------- Gameplay Phase Test -------------------------------------------

void testMainGameLoop() {

    cout << "=============================================\n";
    cout << "      TEST: CONTROLLED MAIN GAME LOOP        \n";
    cout << "=============================================\n\n";

    mainGameLoop();

    cout << "=============================================\n";
    cout << "          END OF MAIN GAME LOOP TEST         \n";
    cout << "=============================================\n\n";

}

/*---------------------------------- Automated Simulation Driver ----------------------------------*/

void mainGameLoop() {

    GameEngine* gameEngine = new GameEngine();
    gameEngine -> mainGameLoop();
    delete gameEngine;

}