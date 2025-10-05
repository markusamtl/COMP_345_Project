#include "GameEngineDriver.h"

using namespace WarzoneEngine;

void testGameStates() {
    GameEngine engine;

    std::cout << "=== GameEngine Console ===\n";
    std::cout << "Commands follow the diagram. Examples:\n"
                 "  loadmap ../Map/test_maps/Brazil/Brazil.map\n"
                 "  validatemap\n"
                 "  addplayer Alice\n"
                 "  addplayer Bob\n"
                 "  gamestart\n"
                 "  assignreinforcement\n"
                 "  issueorder\n"
                 "  endissueorders\n"
                 "  execorder\n"
                 "  endexecorders\n"
                 "  end\n\n";

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

        if (cmd == "end") break;
    }

    std::cout << "=== Engine ended ===\n";
}

int main() {
    testGameStates();
    return 0;
}
