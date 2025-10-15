#include "CommandProcessingDriver.h"

void testCommandProcessorCLI() {

    //---------------------------- Setup Phase ----------------------------
    GameEngine* engine = new GameEngine();
    CommandProcessor* processor = new CommandProcessor(engine);

    cout << "=============================================\n";
    cout << "          TEST: COMMAND PROCESSOR (CLI)      \n";
    cout << "=============================================\n\n";

    cout << "[Driver] This test demonstrates the CommandProcessor in console mode.\n";
    //---------------------------- Initialization ----------------------------
    bool running = true;
    ostringstream sessionLog; // Accumulates all console I/O before gameplayPhase

    //---------------------------- Command Loop ----------------------------
    while(running){

        //---------------------- Show valid commands for current state ----------------------//
        EngineState state = engine -> getState();

        cout << "\n[Driver] Current State: " << engine -> getStateAsString() << endl;
        sessionLog << "\n[Driver] Current State: " << engine -> getStateAsString() << endl;

        cout << "[Driver] Available commands: ";
        sessionLog << "[Driver] Available commands: ";

        switch(state){

            case EngineState::Start:
                cout << "loadmap <mapfile>\n";
                sessionLog << "loadmap <mapfile>\n";
                break;

            case EngineState::MapLoaded:
                cout << "validatemap\n";
                sessionLog << "validatemap\n";
                break;

            case EngineState::MapValidated:
                cout << "addplayer <playername>\n";
                sessionLog << "addplayer <playername>\n";
                break;

            case EngineState::PlayersAdded:
                cout << "addplayer <playername>  OR  gamestart\n";
                sessionLog << "addplayer <playername>  OR  gamestart\n";
                break;

            case EngineState::Win:
                cout << "Replay  OR  Quit\n";
                sessionLog << "Replay  OR  Quit\n";
                break;

            case EngineState::AssignReinforcement:
            case EngineState::IssueOrders:
            case EngineState::ExecuteOrders:
                cout << "Game running...\n";
                sessionLog << "Game running...\n";
                break;

            default:
                cout << "Quit\n";
                sessionLog << "Quit\n";
                break;
        }

        cout << "[Driver] Please enter your command: " << endl;
        sessionLog << "[Driver] Please enter your command:\n";

        //Get command from processor
        Command* cmd = processor -> getCommand();
        if(cmd == nullptr){ continue; }
        else{ sessionLog << cmd -> toString(); }

        string cmdName = cmd -> getCommandName();
        vector<string> args = cmd -> getCommandArgs();
        long long cmdHash = StringHandling::hashStringToNum(cmdName);
        ostringstream output;

        //Validate command against current GameEngine state
        if(!processor -> validate(cmd)){

            string effect = "[Driver] Invalid command '" + cmd -> toString() + "' in state: " + engine -> getStateAsString() + "\n";
            cmd -> setEffect(effect);
            cout << effect;
            sessionLog << effect;
            continue;

        }

        //---------------------- Command Execution (Hash-Based Switch) ----------------------//
        switch(cmdHash){

            case LOADMAP_COMMAND_HASH: {

                if(args.empty()){

                    output << "[Driver] Error: Missing map file argument.\n";
                    break;

                }

                string result = engine -> engineLoadMap(args[0], false);
                output << result;

                if(result.find("failed") != string::npos || result.find("error") != string::npos){

                    output << "\n[Driver] Map loading failed. Returning to Start state.\n";
                    engine -> setState(EngineState::Start);

                } else {

                    engine -> setState(EngineState::MapLoaded);

                }

                break;

            }

            case VALIDATEMAP_COMMAND_HASH: {

                string result = engine -> engineValidateMap(false);
                output << result << endl;

                if(result.find("invalid") != string::npos || result.find("failed") != string::npos){

                    output << "[Driver] Map validation failed. Returning to Start state.\n";
                    engine -> setState(EngineState::Start);

                } else {

                    engine -> setState(EngineState::MapValidated);

                }

                break;

            }

            case ADDPLAYER_COMMAND_HASH: {

                if(args.empty()){

                    output << "[Driver] Error: Missing player name argument.\n";
                    break;

                //Make sure that the number of players added does not exceed the maximum number of territories in the map
                } else if(engine -> getGameMap() != nullptr && 
                          engine -> getPlayers().size() == engine -> getGameMap() -> getTerritories().size()) {

                    output << "[Driver] Error: Too many player added: Starting the game now!" << endl;
                    engine -> setState(EngineState::PlayersAdded);

                }

                string result = engine -> engineAddPlayer(args[0], false);
                output << result << endl;
                engine -> setState(EngineState::PlayersAdded);
                break;

            }

            case GAMESTART_COMMAND_HASH: {

                //Make sure at least 2 players exist in the game before starting
                if(engine -> getPlayers().size() < 2) {

                    output << "[Driver] Error: You need at LEAST 2 players to play a game.\n" << endl;
                    engine -> setState(EngineState::PlayersAdded);
                    break;

                }

                string result = engine -> engineGameStart(false);
                output << result << endl;

                // Pass session log + file path to gameplayPhase
                engine -> gameplayPhase(false, sessionLog.str(), "../CommandProcessing/Games/");
                sessionLog.str("");
                sessionLog.clear();
                break;

            }

            case REPLAY_COMMAND_HASH: {

                output << "[Driver] Restarting game session.\n" << endl;
                engine -> setState(EngineState::Start);

                //Clear string logs
                sessionLog.str("");
                sessionLog.clear();

                break;

            }

            case QUIT_COMMAND_HASH: {

                output << "[Driver] Exiting game. Goodbye!\n" << endl;
                engine -> setState(EngineState::End);
                running = false;
                break;

            }

            default: {

                output << "[Driver] Unknown or unsupported command: " << cmdName << "\n" << endl;
                break;

            }

        }

        //---------------------- Log & Display ----------------------//
        string effectText = output.str();
        cout << effectText;
        sessionLog << effectText;
        cmd -> setEffect(effectText);

    }

    //---------------------------- Cleanup ----------------------------
    delete processor;
    delete engine;

    cout << "[Driver] Simulation complete. Gameplay logs handled by GameEngine.\n";

}
