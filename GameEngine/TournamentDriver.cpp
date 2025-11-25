#include "GameEngine/GameEngine.h"
#include "LoggingObserver/LoggingObserver.h"
#include <iostream>
#include <vector>
#include <string>

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

    // SAMPLE INPUTS - adjust paths if your map files live elsewhere
    std::vector<std::string> maps = {
        "../Map/test_maps/Brazil/Brazil.map",
        "../Map/test_maps/Canada/Canada.map"
    };

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
