#include "GameEngineDriver.h"

using namespace WarzoneEngine;

void testGameStates() {
    GameEngine engine;

    cout << "=== WARZONE GAME ENGINE CONSOLE ===\n";
    cout << "Available commands (case-insensitive):\n";
    cout << "  loadmap <path>            -> Load a map file\n";
    cout << "  validatemap               -> Validate the currently loaded map\n";
    cout << "  addplayer <name>          -> Add a player to the game\n";
    cout << "  gamestart                 -> Assign territories and start the game\n";
    cout << "  assignreinforcement       -> Compute and assign reinforcements to all players\n";
    cout << "  issueorder                -> Each player issues orders\n";
    cout << "  endissueorders            -> End issuing phase, move to execution\n";
    cout << "  executeorder              -> Execute all orders interleaved by player\n";
    cout << "  endexecuteorder           -> End execution phase (loops back to reinforcement)\n";
    cout << "  win                       -> Show winning state (if applicable)\n";
    cout << "  end                       -> Terminate the engine\n";
    cout << "\nExample flow:\n";
    cout << "  loadmap ../Map/test_maps/Brazil/Brazil.map\n";
    cout << "  validatemap\n";
    cout << "  addplayer Alice\n";
    cout << "  addplayer Bob\n";
    cout << "  gamestart\n";
    cout << "  assignreinforcement\n";
    cout << "  issueorder\n";
    cout << "  endissueorders\n";
    cout << "  executeorder\n";
    cout << "  endexecuteorder\n";
    cout << "  end\n";
    cout << "====================================\n\n";

    string line;
    while (engine.getState() != EngineState::End && std::getline(cin, line)) {
        if (line.empty()) continue;

        std::stringstream ss(line);
        std::string cmd, arg;
        ss >> cmd;
        std::getline(ss, arg);
        if (!arg.empty() && arg[0] == ' ') arg.erase(0, 1);

        string result = engine.processCommand(cmd, arg);
        cout << "[State] " << engine.getState() << " | " << result << "\n";

        if (cmd == "end" || cmd == "END") break;
    }

    cout << "\n=== GAME ENGINE TERMINATED ===\n";
}

int main() {
    testGameStates();
    return 0;
}
