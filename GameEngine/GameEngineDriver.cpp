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

#include <chrono>
#include <iomanip>
#include <sstream>
#include <ctime>

void simulateRealGame() {

    bool play = true;
    bool playAgain = true;
    bool printOutputs = true;

    string playGame;
    cout << "\nWould you like to simulate a full automatic game sequence now? (yes/no): ";
    getline(cin, playGame);

    // Normalize input
    playGame.erase(std::remove_if(playGame.begin(), playGame.end(), ::isspace), playGame.end());
    transform(playGame.begin(), playGame.end(), playGame.begin(), ::tolower);

    if (playGame == "yes" || playGame == "y") {

        play = true;
        cout << "\n[Driver] Starting full game simulation...\n";
    } else if (playGame == "no" || playGame == "n") {
        
        cout << "\n[Driver] Simulation skipped. Exiting program.\n";
        return;

    } else {
        
        cout << "\n[Driver] Invalid input. Please type 'yes' or 'no' next time. Exiting.\n";
        return;
    
    }

    if(play) {
        
        while(playAgain) {
            
            // Generate timestamped filename for each new game
            auto now = chrono::system_clock::now(); //Get current time
            time_t nowTime = chrono::system_clock::to_time_t(now); //Cast time
            tm localTime = *localtime(&nowTime); //Format time to system timezone

            ostringstream fileNameBuilder;
            fileNameBuilder << "simulation_log_" << std::put_time(&localTime, "%Y_%m_%d_%H_%M_%S") << ".txt";
            string logFileName = fileNameBuilder.str();

            // Open log file
            ofstream logFile(logFileName);
            if (!logFile.is_open()) {

                cerr << "[Error] Could not open " << logFileName << " for writing.\n";
                return;
            
            }

            // Backup original cout buffer and redirect output to file
            streambuf* originalCoutBuffer = cout.rdbuf();
            cout.rdbuf(logFile.rdbuf());
            
            cout << "\n=== SIMULATING FULL GAME SEQUENCE ===\n";
            GameEngine engine; //Instantiate engine

            //Load and validate map
            cout << engine.loadmap("../Map/test_maps/Brazil/Brazil.map") << '\n';
            cout << engine.validatemap() << '\n';

            //Add players
            cout << engine.addplayer("Alice") << '\n';
            cout << engine.addplayer("Bob") << '\n';
            cout << engine.addplayer("Carl") << '\n';
            cout << engine.addplayer("Dan") << '\n';
            cout << engine.gamestart() << '\n'; //Adds neutral player by default, automaticall assigns territories to all players 

            // Ask for turn cap
            int maxTurns = 0;
            while(true) {

                cout.rdbuf(originalCoutBuffer);
                cout << "\nEnter maximum number of turns to simulate: ";
                string maxTurnsInput;
                getline(cin, maxTurnsInput);

                std::cout.rdbuf(logFile.rdbuf()); //Log input
                std::cout << "[User Input] " << maxTurnsInput << "\n";

                try {

                    maxTurns = stoi(maxTurnsInput);
                    if(maxTurns < 1){ throw invalid_argument("non-positive"); } //Make sure inputted number is < 1
                    break;

                } catch (...) {

                    cout.rdbuf(originalCoutBuffer); //Return to console
                    cout << "Invalid input. Please enter a positive integer.\n";
                    cout.rdbuf(logFile.rdbuf()); //Return to log
                    cout << "Invalid input detected.\n";
                
                }
            
            }

            string printOutput;

            while(true) {

                cout.rdbuf(originalCoutBuffer); //Switch to console
                cout << "\nWould you like to suppress information printed every round? (yes/no): ";
                getline(cin, printOutput);

                cout.rdbuf(logFile.rdbuf()); //Switch to log
                cout << "[User Input] " << printOutput << "\n";

                transform(printOutput.begin(), printOutput.end(), printOutput.begin(), ::tolower);
                printOutput.erase(remove_if(printOutput.begin(), printOutput.end(), ::isspace), printOutput.end());

                if (printOutput == "yes" || printOutput == "y") {

                    printOutputs = false;
                    break;

                } else if (printOutput == "no" || printOutput == "n") {
                    
                    printOutputs = true;
                    break;

                } else {

                    cout.rdbuf(originalCoutBuffer);
                    cout << "Invalid input. Please type 'yes' or 'no'.\n";
                    cout.rdbuf(logFile.rdbuf()); //Write to log
                    cout << "Invalid input detected.\n";
                
                }

            }

            //NOTE: For the rest of the game, everything is written to the log file. Only winning / game termination shows up on console now

            int turn = 1;
            auto simulationStart = std::chrono::high_resolution_clock::now(); //Start tracking time it takes to execute a game

            //Main game loop: Looping between AssignReinforcment, IssueOrders, and ExecuteOrders.
            //Terminates when someone wins / max turns are reached
            while(engine.getState() != EngineState::End && engine.getState() != EngineState::Win && turn <= maxTurns) {
                
                cout << "\n--- TURN " << turn << " ---\n";

                if(engine.getState() == EngineState::AssignReinforcement) { //AssignReinforcment state
                    
                    if(printOutputs){ cout << engine.assignreinforcement() << '\n';}
                    else{engine.assignreinforcement(); }

                }

                if(engine.getState() == EngineState::IssueOrders) { //IssueOrders state
                    
                    if(printOutputs) {
                        
                        cout << engine.issueorder() << '\n';
                        cout << engine.endissueorders() << '\n';
                    
                    } else {
                        
                        engine.issueorder();
                        engine.endissueorders();
                    
                    }

                }

                if (engine.getState() == EngineState::ExecuteOrders) { //ExecuteOrders state
                    
                    if (printOutputs) {

                        cout << engine.executeOrder() << '\n';
                        //Move on to end of order execution if no one wins
                        if(engine.getState() == EngineState::ExecuteOrders){ cout << engine.endExecuteOrder() << '\n'; }
                    
                    } else {

                        engine.executeOrder();
                        //Move on to end of order execution if no one wins
                        if(engine.getState() == EngineState::ExecuteOrders){ engine.endExecuteOrder(); }
                    
                    }

                }

                turn++;

            }

            // End conditions
            string result;
            
            //Someone won
            if(engine.getState() == EngineState::Win){ result = "\n[Driver] Win state reached.\n"; }
            
            //For some reason, the engine stopped (should not occur)
            else if (engine.getState() == EngineState::End){result = "\n[Driver] Engine ended.\n";}
            
            //Turn limit was reached
            else{result = "\n[Driver] Max turn cap reached. Stopping simulation.\n";}

            cout << result;

            cout.rdbuf(originalCoutBuffer); //Return to console
            cout << result;
            cout.rdbuf(logFile.rdbuf()); //Switch to log

            auto simulationEnd = chrono::high_resolution_clock::now(); //Get ending time

            //Get execution time in ms
            auto duration = chrono::duration_cast<chrono::milliseconds>(simulationEnd - simulationStart).count();

            cout << "\n=== SIMULATION COMPLETE ===\n";
            cout << "Total simulation time: " << duration << " ms\n";

            cout.rdbuf(originalCoutBuffer); //Switch to console
            cout << "\n=== SIMULATION COMPLETE ===\n";
            cout << "Total simulation time: " << duration << " ms\n";

            string replay;
            while(true) {

                cout << "\nWould you like to simulate again? (yes/no): ";
                getline(cin, replay);

                //Normalize input
                transform(replay.begin(), replay.end(), replay.begin(), ::tolower);
                replay.erase(remove_if(replay.begin(), replay.end(), ::isspace), replay.end());

                if (replay == "yes" || replay == "y") {

                    playAgain = true;
                    break;

                } else if (replay == "no" || replay == "n") {

                    playAgain = false;
                    break;

                } else {

                    cout.rdbuf(originalCoutBuffer);
                    cout << "Invalid input. Please type 'yes' or 'no'.\n";
                  
                }

            }

            //Close log after each game
            logFile.close();
            cout << "\n[Driver] Simulation log has been saved to '" << logFileName << "'.\n";

            if(playAgain){ cout << "\n\n=== RESTARTING SIMULATION ===\n"; }
            else{ cout << "\n=== END OF ALL SIMULATIONS ===\n";}
        }

    } else {

        cout << "\n[Driver] Simulation skipped. Exiting program.\n";
    
    }

}