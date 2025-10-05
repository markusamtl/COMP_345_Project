#include "GameEngine.h"
#include <sstream>
#include <algorithm>

namespace WarzoneEngine {

// ------------------ helpers ------------------

std::ostream& operator<<(std::ostream& os, const EngineState& s) {
    switch (s) {
        case EngineState::Start:               os << "start"; break;
        case EngineState::MapLoaded:           os << "map loaded"; break;
        case EngineState::MapValidated:        os << "map validated"; break;
        case EngineState::PlayersAdded:        os << "players added"; break;
        case EngineState::AssignReinforcement: os << "assign reinforcement"; break;
        case EngineState::IssueOrders:         os << "issue orders"; break;
        case EngineState::ExecuteOrders:       os << "execute orders"; break;
        case EngineState::Win:                 os << "win"; break;
        case EngineState::End:                 os << "end"; break;
    }
    return os;
}

GameEngine::GameEngine()
: state(EngineState::Start), gameMap(nullptr), deck(nullptr) {}

GameEngine::~GameEngine() { clearGame(); }

void GameEngine::clearGame() {
    delete deck; deck = nullptr;
    for (Player* p : players) delete p;
    players.clear();
    delete gameMap; gameMap = nullptr;
    reinforcementPool.clear();
}

bool GameEngine::requireState(EngineState expected, const string& cmd) {
    if (state != expected) {
        std::cerr << "[Error] Command '" << cmd << "' is invalid in state '" << state << "'.\n";
        return false;
    }
    return true;
}

// ------------------ transitions / commands ------------------

string GameEngine::loadmap(const string& path) {
    if (!requireState(EngineState::Start, "loadmap")) return "invalid";

    clearGame(); // start fresh if user loads again

    MapLoader loader;
    int rc = loader.importMapInfo(path);
    if (rc != 0) {
        std::cerr << "  importMapInfo failed, code=" << rc << "\n";
        return "loadmap: failed";
    }

    auto [ok, built] = loader.loadMap();
    if (ok != 0 || built == nullptr) {
        std::cerr << "  loadMap failed, code=" << ok << "\n";
        return "loadmap: failed";
    }

    gameMap = built;
    state = EngineState::MapLoaded;
    return "map loaded";
}

string GameEngine::validatemap() {
    if (!requireState(EngineState::MapLoaded, "validatemap")) return "invalid";
    if (!gameMap) return "no map";

    bool good = gameMap->validate();
    if (!good) return "map invalid";

    state = EngineState::MapValidated;
    return "map validated";
}

string GameEngine::addplayer(const string& name) {
    // allow both MapValidated and PlayersAdded
    if (state != EngineState::MapValidated && state != EngineState::PlayersAdded) {
        std::cerr << "[Error] Command 'addplayer' is invalid in state '" << state << "'.\n";
        return "invalid";
    }

    if (name.empty()) {
        std::cerr << "[Error] Player name cannot be empty.\n";
        return "invalid";
    }

    // create player
    unordered_map<WarzoneMap::Continent*, long long> playerHash = gameMap->buildEmptyContinentHashmap();
    players.push_back(new WarzonePlayer::Player(name, playerHash));
    std::cout << "Player added: " << name << "\n";

    // transition only on the first player
    state = EngineState::PlayersAdded;
    return "players added";
}


string GameEngine::gamestart() {
    // can be called from PlayersAdded after adding at least 2 players
    if (!requireState(EngineState::PlayersAdded, "gamestart")) return "invalid";
    if (players.size() < 2) return "need at least 2 players";
    if (!gameMap) return "no map";

    // 1) Distribute territories round-robin
    assignCountries();

    // 2) Give each player a starting hand of 2 random cards (optional nicety)
    for (Player* p : players) {
        if (deck) {
            Card* c1 = deck->draw();
            Card* c2 = deck->draw();
            if (c1) p->getHand()->addCardToHand(c1);
            if (c2) p->getHand()->addCardToHand(c2);
        }
    }

    // 3) Move to assign reinforcement
    state = EngineState::AssignReinforcement;
    return "assign reinforcement";
}

void GameEngine::assignCountries() {
    vector<Territory*> terrs = gameMap->getTerritories();
    if (terrs.empty()) return;

    // naive shuffle using time entropy (consistent with your TimeUtil)
    std::sort(terrs.begin(), terrs.end(),
              [](Territory* a, Territory* b){ return a->getID() < b->getID(); });
    for (size_t i = 0; i < terrs.size(); ++i) {
        size_t idx = static_cast<size_t>( (TimeUtil::getSystemTimeNano() + static_cast<long long>(i)) % terrs.size() );
        std::swap(terrs[i], terrs[idx]);
    }

    // Round-robin assign
    unordered_map<Continent*, long long> refTable = gameMap->getContinentLookupTable();

    size_t pi = 0;
    for (Territory* t : terrs) {
        Player* owner = players[pi];
        owner->addOwnedTerritories(t);
        pi = (pi + 1) % players.size();
    }

    // After distribution, refresh each player's continent progress table
    for (Player* p : players) {
        p->setContinentLookupTablePlayer(gameMap->buildEmptyContinentHashmap());
        for (Territory* t : p->getOwnedTerritories().getTerritories()) {
            Continent* c = t->getContinent();
            if (c) {
                auto tbl = p->getContinentLookupTablePlayer();
                // (we can safely recompute using addOwnedTerritories already updates table,
                // but the explicit set above zeroed it, so re-add here)
            }
        }
        // Add a tiny starting army to every territory to avoid zeros
        for (Territory* t : p->getOwnedTerritories().getTerritories())
            if (t) t->setNumArmies(std::max(1, t->getNumArmies()));
    }
}

int GameEngine::computeReinforcementFor(Player* p) const {
    // Base: max(3, floor(#owned / 3))
    int owned = static_cast<int>(p->getOwnedTerritories().size());
    int base = std::max(3, owned / 3);

    // Continent bonuses when player controls the continent
    int bonus = 0;
    const auto& ref = gameMap->getContinentLookupTable();
    for (Continent* c : gameMap->getContinents()) {
        if (!c) continue;
        if (p->controlsContinent(ref, c)) {
            bonus += c->getBonusValue();
        }
    }
    return base + bonus;
}

string GameEngine::assignreinforcement() {
    if (!requireState(EngineState::AssignReinforcement, "assignreinforcement")) return "invalid";
    if (!gameMap) return "no map";

    reinforcementPool.clear();

    // compute and store
    for (Player* p : players) {
        int r = computeReinforcementFor(p);
        reinforcementPool[p] = r;
        std::cout << "[Reinforcement] " << p->getPlayerName()
                  << " receives " << r << " armies (own=" << p->getOwnedTerritories().size()
                  << ", min3 + continent bonuses).\n";
    }

    // For a minimal integrated demo, automatically seed Deploy orders equal to the reinforcements.
    autoSeedOrdersFromReinforcements();

    state = EngineState::IssueOrders;
    return "issue orders";
}

void GameEngine::autoSeedOrdersFromReinforcements() {
    // Very simple policy: dump all reinforcement on the first territory to defend (if any)
    for (Player* p : players) {
        int pool = reinforcementPool[p];
        if (pool <= 0) continue;

        vector<Territory*> defend = p->toDefend();
        if (defend.empty() || !defend[0]) continue;

        p->getPlayerOrders()->addOrder(new Deploy(p, defend[0], pool));
    }
}

string GameEngine::issueorder() {
    if (!requireState(EngineState::IssueOrders, "issueorder")) return "invalid";

    // For demo: let each player ALSO issue one random order (your Player::issueOrder)
    for (Player* p : players) p->issueOrder();

    return "issuing orders";
}

string GameEngine::endissueorders() {
    if (!requireState(EngineState::IssueOrders, "endissueorders")) return "invalid";
    state = EngineState::ExecuteOrders;
    return "execute orders";
}

string GameEngine::execorder() {
    if (!requireState(EngineState::ExecuteOrders, "execorder")) return "invalid";

    // naive sequential resolution: player 0 executes all, then 1, etc.
    // (good enough for Part 5; you can implement interleaving later)
    for (Player* p : players) {
        auto& list = p->getPlayerOrders()->getOrders();
        while (!list.empty()) {
            WarzoneOrder::Order* o = list[0];
            o->execute();
            p->getPlayerOrders()->removeOrder(0);
        }
    }
    return "executing orders";
}

string GameEngine::endexecorders() {
    if (!requireState(EngineState::ExecuteOrders, "endexecorders")) return "invalid";

    // Check win condition: someone controls all continents
    const auto& ref = gameMap->getContinentLookupTable();
    for (Player* p : players) {
        if (p->hasWon(ref)) {
            state = EngineState::Win;
            std::cout << ">>> " << p->getPlayerName() << " controls all continents! <<<\n";
            return "win";
        }
    }

    // If no winner, loop back to reinforcement (new turn)
    state = EngineState::AssignReinforcement;
    return "assign reinforcement";
}

string GameEngine::win() {
    if (!requireState(EngineState::Win, "win")) return "invalid";
    return "win";
}

string GameEngine::end() {
    if (state == EngineState::End) return "end";
    state = EngineState::End;
    return "end";
}

// ------------------ command dispatcher ------------------

string GameEngine::processCommand(const string& raw, const string& arg) {
    string cmd = raw;
    // lower-case normalize for convenience
    for (char& c : cmd) c = static_cast<char>(std::tolower(c));

    if (cmd == "loadmap") return loadmap(arg);
    if (cmd == "validatemap") return validatemap();
    if (cmd == "addplayer") return addplayer(arg);
    if (cmd == "gamestart") return gamestart();

    if (cmd == "assignreinforcement") return assignreinforcement();
    if (cmd == "issueorder") return issueorder();
    if (cmd == "endissueorders") return endissueorders();
    if (cmd == "execorder") return execorder();
    if (cmd == "endexecorders") return endexecorders();

    if (cmd == "win") return win();
    if (cmd == "end") return end();

    std::cerr << "[Error] Unknown command: " << raw << "\n";
    return "invalid";
}

} // namespace WarzoneEngine
