#include "GameEngineDriver.h"
#include <algorithm>
#include <iostream>
#include <sstream>
#include <chrono>

using namespace WarzoneEngine;

/*---------------------------------- Manual Console Driver ----------------------------------*/

void testGameStates() {
    GameEngine engine;

    std::cout << "=== WARZONE GAME ENGINE CONSOLE ===\n";
    std::cout << "Available commands (case-insensitive):\n";
    std::cout << "  loadmap <path>            -> Load a map file\n";
    std::cout << "  validatemap               -> Validate the currently loaded map\n";
    std::cout << "  addplayer <name>          -> Add a player to the game\n";
    std::cout << "  gamestart                 -> Assign territories and start the game\n";
    std::cout << "  assignreinforcement       -> Compute and assign reinforcements to all players\n";
    std::cout << "  issueorder                -> Each player issues orders\n";
    std::cout << "  endissueorders            -> End issuing phase, move to execution\n";
    std::cout << "  executeorder              -> Execute all orders interleaved by player\n";
    std::cout << "  endexecuteorder           -> End execution phase (loops back to reinforcement)\n";
    std::cout << "  win                       -> Show winning state (if applicable)\n";
    std::cout << "  end                       -> Terminate the engine\n";
    std::cout << "\nExample flow:\n";
    std::cout << "  loadmap ../Map/test_maps/Brazil/Brazil.map\n";
    std::cout << "  validatemap\n";
    std::cout << "  addplayer Alice\n";
    std::cout << "  addplayer Bob\n";
    std::cout << "  gamestart\n";
    std::cout << "  assignreinforcement\n";
    std::cout << "  issueorder\n";
    std::cout << "  endissueorders\n";
    std::cout << "  executeorder\n";
    std::cout << "  endexecuteorder\n";
    std::cout << "  end\n";
    std::cout << "====================================\n\n";

    std::string line;
    while (engine.getState() != EngineState::End && std::getline(std::cin, line)) {
        if (line.empty()) continue;

        std::stringstream ss(line);
        std::string cmd, arg;
        ss >> cmd;
        std::getline(ss, arg);
        if (!arg.empty() && arg[0] == ' ') arg.erase(0, 1);

        std::string result = engine.processCommand(cmd, arg);
        std::cout << "[State] " << engine.getState() << " | " << result << "\n";

        if (cmd == "end" || cmd == "END") break;
    }

    std::cout << "\n=== GAME ENGINE TERMINATED ===\n";
}

/*---------------------------------- Automated Simulation Driver ----------------------------------*/

void simulateRealGame() {

    bool play = true;
    bool playAgain = true;
    bool printOutputs = true;

    std::string response;
    std::cout << "\nWould you like to simulate a full automatic game sequence now? (yes/no): ";
    std::getline(std::cin, response);

    // Normalize input
    response.erase(std::remove_if(response.begin(), response.end(), ::isspace), response.end());
    std::transform(response.begin(), response.end(), response.begin(), ::tolower);

    if (response == "yes" || response == "y") {

        play = true;
        std::cout << "\n[Driver] Starting full game simulation...\n";

    } else if (response == "no" || response == "n") {
        std::cout << "\n[Driver] Simulation skipped. Exiting program.\n";
        return;
    } else {
        std::cout << "\n[Driver] Invalid input. Please type 'yes' or 'no' next time. Exiting.\n";
        return;
    }

    if(play) {

        while(playAgain) {
            
            cout << "\n=== SIMULATING FULL GAME SEQUENCE ===\n";

            GameEngine engine;

            // Load and validate map
            cout << engine.loadmap("../Map/test_maps/Brazil/Brazil.map") << '\n';
            cout << engine.validatemap() << '\n';

            // Add players
            cout << engine.addplayer("Alice") << '\n';
            cout << engine.addplayer("Bob") << '\n';
            cout << engine.addplayer("Carl") << '\n';
            cout << engine.addplayer("Dan") << '\n';
            cout << engine.gamestart() << '\n';

            // ------------------------------
            // Ask user for turn cap
            // ------------------------------
            int maxTurns = 0;
            while(true) {

                std::cout << "\nEnter maximum number of turns to simulate: ";
                std::string input;
                std::getline(std::cin, input);

                try {

                    maxTurns = std::stoi(input);
                    if(maxTurns <= 0){ throw std::invalid_argument("non-positive"); }
                    break;

                }catch(...){ cout << "Invalid input. Please enter a positive integer.\n"; }
            
            }

            string printOutput;
            while (true) {

                std::cout << "\nWould you like to surpress information printed every round? (yes/no): ";
                std::getline(std::cin, printOutput);
                std::transform(printOutput.begin(), printOutput.end(), printOutput.begin(), ::tolower);
                printOutput.erase(std::remove_if(printOutput.begin(), printOutput.end(), ::isspace), printOutput.end());

                if (printOutput == "yes" || printOutput == "y") {

                    printOutputs = true;
                    break;
                
                } else if (printOutput == "no" || printOutput == "n") {
                    
                    printOutputs = false;
                    break;
                
                } else { cout << "Invalid input. Please type 'yes' or 'no'.\n"; }

            }

            int turn = 1;

            // ------------------------------
            // Main simulation loop
            // ------------------------------

            // Start timer
            auto simulationStart = chrono::high_resolution_clock::now(); //Track total simulation time

            while(engine.getState() != EngineState::End && engine.getState() != EngineState::Win && turn <= maxTurns) {

                std::cout << "\n--- TURN " << turn << " ---\n";

                // Assign Reinforcement phase
                if(engine.getState() == EngineState::AssignReinforcement) {
                    
                    if(printOutputs){ std::cout << engine.assignreinforcement() << '\n'; }
                    else { engine.assignreinforcement(); }

                }

                // Issue Orders phase
                if (engine.getState() == EngineState::IssueOrders) {

                    if(printOutputs){ 

                        cout << engine.issueorder() << '\n';
                        cout << engine.endissueorders() << '\n';

                    } else { 

                        engine.issueorder();
                        engine.endissueorders();

                    }
                    
                }

                // Execute Orders phase
                if (engine.getState() == EngineState::ExecuteOrders) {
                    
                    if(printOutputs){

                        cout << engine.executeOrder() << '\n';
                        if(engine.getState() == EngineState::ExecuteOrders) { cout << engine.endExecuteOrder() << '\n'; }

                    } else {

                        engine.executeOrder();
                        if(engine.getState() == EngineState::ExecuteOrders) { engine.endExecuteOrder(); }

                    }

                }

                turn++;
            
            }

            // ------------------------------
            // End conditions
            // ------------------------------
            if(engine.getState() == EngineState::Win) { cout << "\n[Driver] Win state reached.\n"; }
            else if(engine.getState() == EngineState::End) { cout << "\n[Driver] Engine ended.\n"; }
            else if (turn > maxTurns) { cout << "\n[Driver] Max turn cap (" << maxTurns << ") reached. Stopping simulation.\n"; }

            // Stop timer
            auto simulationEnd = chrono::high_resolution_clock::now();
            auto duration = chrono::duration_cast<std::chrono::milliseconds>(simulationEnd - simulationStart).count();

            // Display total time
            std::cout << "\n=== SIMULATION COMPLETE ===\n";
            std::cout << "Total simulation time: " << duration << " ms\n";

            // ------------------------------
            // Ask user if they want to play again
            // ------------------------------
            string replay;
            while (true) {

                cout << "\nWould you like to simulate again? (yes/no): ";
                getline(std::cin, replay);
                transform(replay.begin(), replay.end(), replay.begin(), ::tolower);
                replay.erase(remove_if(replay.begin(), replay.end(), ::isspace), replay.end());

                if (replay == "yes" || replay == "y") {
                    
                    playAgain = true;
                    break;

                } else if (replay == "no" || replay == "n") {
                
                    playAgain = false;
                    break;
                
                } else { cout << "Invalid input. Please type 'yes' or 'no'.\n"; }

            }

            if (playAgain) { cout << "\n\n=== RESTARTING SIMULATION ===\n"; }
            else { cout << "\n=== END OF ALL SIMULATIONS ===\n"; }
        
        }

    } else { std::cout << "\n[Driver] Simulation skipped. Exiting program.\n"; }

}