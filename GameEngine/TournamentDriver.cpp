#include "GameEngine/GameEngine.h"
#include "LoggingObserver/LoggingObserver.h"
#include <iostream>
#include <vector>
#include <string>
#include <filesystem>

using namespace WarzoneEngine;
using namespace WarzoneLog;
using std::cout;
using std::endl;

/**
 * testTournament
 *
 * A self-contained automated test for Part 2 (Tournament Mode).
 * It constructs a small set of maps and non-interactive strategies,
 * runs the tournament via GameEngine::runTournament(...), attaches a
 * LogObserver so events are written to the game log, and prints a
 * readable results table to stdout.
 */
void testTournament() {

    GameEngine engine;
    LogObserver logger;
    engine.attach(&logger);

    // SAMPLE INPUTS - map paths relative to project root
    std::vector<std::string> maps = {
        "Map/test_maps/Brazil/Brazil.map",
        "Map/test_maps/Canada/Canada.map"
    };

    // Resolve map paths flexibly so the test works when the binary is
    // executed from either the project root or the build directory.
    namespace fs = std::filesystem;
    auto resolvePath = [&](const std::string &p)->std::string{
        fs::path candidate(p);
        if (fs::exists(candidate)) return candidate.string();
        // Try relative from build directory (one level up)
        fs::path alt = fs::path("../") / candidate;
        if (fs::exists(alt)) return alt.string();
        // Try stripping leading ../ if present
        std::string s = p;
        if (s.rfind("../", 0) == 0){
            fs::path stripped = s.substr(3);
            if (fs::exists(stripped)) return stripped.string();
        }
        // Try ./ prefix
        fs::path dot = fs::path("./") / candidate;
        if (fs::exists(dot)) return dot.string();
        // Not found — return original so engine will report an error
        return p;
    };

    // Apply resolution
    for (auto &mp : maps){
        std::string resolved = resolvePath(mp);
        if (resolved != mp){
            std::cout << "[Info] Resolved map path '" << mp << "' -> '" << resolved << "'\n";
            mp = resolved;
        }
    }

    // Tournament must use non-interactive strategies only
    std::vector<std::string> strategies = {
        "Aggressive",
        "Benevolent",
        "Neutral",
        "Cheater"
    };

    int numGames = 2;   // G
    int maxTurns = 30;  // D

    cout << "Running tournament: " << maps.size() << " maps x "
         << numGames << " games, " << strategies.size() << " players, "
         << maxTurns << " max turns per game" << endl;

    auto results = engine.runTournament(maps, strategies, numGames, maxTurns);

    cout << "\nTournament results table (rows = maps, columns = games):\n";
    for (size_t i = 0; i < results.size(); ++i) {
        cout << "Map " << (i+1) << " (" << maps[i] << ") : ";
        for (size_t j = 0; j < results[i].size(); ++j) {
            cout << results[i][j];
            if (j + 1 < results[i].size()) cout << " | ";
        }
        cout << endl;
    }

    engine.detach(&logger);
}
