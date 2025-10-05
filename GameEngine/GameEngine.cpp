#include "GameEngine.h"
#include <sstream>
#include <algorithm>

namespace WarzoneEngine {


    //-- Constructors, Destructor, Copy Constructor, Assignment Operator, Stream Insertion Operator --//
    

    GameEngine::GameEngine() {

        state = EngineState::Start;
        gameMap = nullptr;
        deck = nullptr;
        currentPlayer = nullptr;
    
    }

    GameEngine::~GameEngine() { clearGame(); }

    //TODO: Implement copy constructor 
    //TODO: Implement assignment operator

    std::ostream& operator<<(std::ostream& os, const EngineState& s) {
        
        switch(s) {

            case (EngineState::Start): os << "start"; break;
            case (EngineState::MapLoaded): os << "map loaded"; break;
            case (EngineState::MapValidated): os << "map validated"; break;
            case (EngineState::PlayersAdded): os << "players added"; break;
            case (EngineState::AssignReinforcement): os << "assign reinforcement"; break;
            case (EngineState::IssueOrders): os << "issue orders"; break;
            case (EngineState::ExecuteOrders): os << "execute orders"; break;
            case (EngineState::Win): os << "win"; break;
            case (EngineState::End): os << "end"; break;
        
        }

        return os;
    }

    //-- Accessors and Mutators --//

    //TODO: Implement accessors and mutators for gameMap, deck, players, playerQueue, currentPlayer

    //-- Class Methods --//

    // ------------------ TRANSITIONS ------------------

    bool GameEngine::isCurrentStateCorrect(EngineState expected, const string& cmd) {

        if(state != expected) {

            std::cerr << "[Error] Command '" << cmd << "' invalid in state '" << state << "'.\n";
        
            return false;
        }

        return true;
    
    }

    string GameEngine::loadmap(const string& path) {

        if(!isCurrentStateCorrect(EngineState::Start, "loadmap")) return "loadmap is not a valid state!";
        clearGame();

        MapLoader loader;
        
        // Import map data from file
        int loaderReturnCode = loader.importMapInfo(path);

        if(loaderReturnCode != 0) {

            std::cerr << "  importMapInfo failed, code=" << loaderReturnCode << "\n";
            return "loadmap: failed";
        
        }

        // Build the Map from the loaded data
        pair<int, Map*> loadResults = loader.loadMap();

        if(loadResults.first != 0 || loadResults.second == nullptr) {
            std::cerr << "  loadMap failed, code=" << loadResults.first << "\n";
            return "loadmap: failed";
        }

        gameMap = loadResults.second ;
        state = EngineState::MapLoaded;

        return "Map has been loaded successfully!";

    }

    string GameEngine::validatemap() {

        if(!isCurrentStateCorrect(EngineState::MapLoaded, "validatemap")){ return "validatemap is not a valid state!"; }
        if (!gameMap) return "no map";

        bool good = gameMap->validate();
        if (!good) return "map invalid";

        state = EngineState::MapValidated;
        return "map validated";
    }

    // ------------------ Player queue management ------------------

    void GameEngine::addPlayer(const std::string& playerName) {
        Player* newPlayer = new Player(playerName,
            std::unordered_map<WarzoneMap::Continent*, long long>{});

        players.push_back(newPlayer);
        playerQueue.push(newPlayer);

        std::cout << "Player added: " << playerName << std::endl;
    }

    void GameEngine::addPlayers(int count) {
        for (int i = 1; i <= count; ++i) {
            std::string name = "Player" + std::to_string(i);
            addPlayer(name);
        }
        std::cout << count << " players added successfully.\n";
    }

    Player* GameEngine::getNextPlayer() {
        if (!playerQueue.empty())
            return playerQueue.front();
        return nullptr;
    }

    void GameEngine::nextTurn() {
        if (playerQueue.empty()) {
            currentPlayer = nullptr;
            return;
        }

        Player* front = playerQueue.front();
        playerQueue.pop();
        playerQueue.push(front);
        currentPlayer = playerQueue.front();
    }

    bool GameEngine::hasPlayers() const {
        return !playerQueue.empty();
    }

    // ------------------ core game flow ------------------

    string GameEngine::addplayer(const string& name) {
        if (state != EngineState::MapValidated && state != EngineState::PlayersAdded) {
            std::cerr << "[Error] 'addplayer' invalid in state '" << state << "'.\n";
            return "invalid";
        }

        addPlayer(name);
        state = EngineState::PlayersAdded;
        return "players added";
    }

    string GameEngine::gamestart() {
        if (!isCurrentStateCorrect(EngineState::PlayersAdded, "gamestart")) return "invalid";
        if (playerQueue.size() < 2) return "need at least 2 players";
        if (!gameMap) return "no map";

        // --- Create Neutral Player ---
        unordered_map<WarzoneMap::Continent*, long long> neutralMap = gameMap->buildEmptyContinentHashmap();
        Player* neutral = new Player("Neutral", neutralMap);
        players.push_back(neutral);
        std::cout << "Neutral player added automatically.\n";

        // --- Assign territories to active players (excluding Neutral) ---
        assignCountries();

        // --- Give each active player (excluding Neutral) two starting cards ---
        for (Player* p : players) {
            if (p->getPlayerName() == "Neutral") continue;

            if (deck) {
                Card* c1 = deck->draw();
                Card* c2 = deck->draw();
                if (c1) p->getHand()->addCardToHand(c1);
                if (c2) p->getHand()->addCardToHand(c2);
            }
        }

        state = EngineState::AssignReinforcement;
        return "assign reinforcement";
    }

    // ------------------ Phases ------------------

    void GameEngine::assignCountries() {
        vector<Territory*> terrs = gameMap->getTerritories();
        if (terrs.empty()) return;

        vector<Player*> ordered;
        queue<Player*> copyQueue = playerQueue;

        while (!copyQueue.empty()) {
            ordered.push_back(copyQueue.front());
            copyQueue.pop();
        }

        size_t pi = 0;
        for (Territory* t : terrs) {
            ordered[pi]->addOwnedTerritories(t);
            pi = (pi + 1) % ordered.size();
        }

        for (Player* p : ordered) {
            for (Territory* t : p->getOwnedTerritories().getTerritories())
                if (t) t->setNumArmies(std::max(1, t->getNumArmies()));
        }
    }

    int GameEngine::computeReinforcementFor(Player* p) const {
        int owned = static_cast<int>(p->getOwnedTerritories().size());
        int base = std::max(3, owned / 3);
        int bonus = 0;
        const auto& ref = gameMap->getContinentLookupTable();

        for (Continent* c : gameMap->getContinents()) {
            if (!c) continue;
            if (p->controlsContinent(ref, c))
                bonus += c->getBonusValue();
        }
        return base + bonus;
    }

    string GameEngine::assignreinforcement() {
        if (!isCurrentStateCorrect(EngineState::AssignReinforcement, "assignreinforcement")) return "invalid";
        if (!gameMap) return "no map";

        vector<Player*> ordered;
        auto copyQueue = playerQueue;
        while (!copyQueue.empty()) {
            ordered.push_back(copyQueue.front());
            copyQueue.pop();
        }

        // Directly compute and apply reinforcements
        for (Player* p : ordered) {
            int r = computeReinforcementFor(p);
            std::cout << "[Reinforcement] " << p->getPlayerName()
                      << " receives " << r << " armies.\n";

            vector<Territory*> defend = p->toDefend();
            if (!defend.empty() && defend[0]) {
                p->getPlayerOrders()->addOrder(new Deploy(p, defend[0], r));
            }
        }

        state = EngineState::IssueOrders;
        return "issue orders";
    }

    string GameEngine::issueorder() {
        if (!isCurrentStateCorrect(EngineState::IssueOrders, "issueorder")) return "invalid";
        for (auto& p : players) p->issueOrder();
        return "issuing orders";
    }

    string GameEngine::endissueorders() {
        if (!isCurrentStateCorrect(EngineState::IssueOrders, "endissueorders")) return "invalid";
        state = EngineState::ExecuteOrders;
        return "execute orders";
    }

    // ------------------ interleaved executeOrder ------------------

    string GameEngine::executeOrder() {
        if (!isCurrentStateCorrect(EngineState::ExecuteOrders, "executeorder")) return "invalid";

        std::cout << "\n=== Executing all player orders (interleaved) ===\n";
        vector<Player*> ordered;
        auto copyQueue = playerQueue;
        while (!copyQueue.empty()) {
            ordered.push_back(copyQueue.front());
            copyQueue.pop();
        }

        executeOrders(ordered);
        std::cout << "\n=== All orders executed ===\n";

        const auto& ref = gameMap->getContinentLookupTable();
        for (Player* p : ordered) {
            if (p && p->hasWon(ref)) {
                state = EngineState::Win;
                std::cout << "\n>>> " << p->getPlayerName()
                          << " controls all continents! <<<\n";
                return "win";
            }
        }

        state = EngineState::AssignReinforcement;
        return "assign reinforcement";
    }

    void GameEngine::executeOrders(vector<WarzonePlayer::Player*>& players) {
        bool anyOrderLeft = true;
        while (anyOrderLeft) {
            anyOrderLeft = false;

            for (Player* p : players) {
                if (!p) continue;
                auto* list = p->getPlayerOrders();
                if (!list) continue;

                auto orders = list->getOrders();
                if (orders.empty()) continue;
                anyOrderLeft = true;

                WarzoneOrder::Order* first = orders.front();
                if (first) {
                    std::cout << "[" << p->getPlayerName() << "] executing: "
                              << *first << std::endl;
                    first->execute();
                    list->removeOrder(first);
                }
            }
        }
    }

    string GameEngine::endExecuteOrder() {
        if (!isCurrentStateCorrect(EngineState::ExecuteOrders, "endexecuteorder")) return "invalid";
        state = EngineState::AssignReinforcement;
        return "assign reinforcement";
    }

    string GameEngine::win() {
        if (!isCurrentStateCorrect(EngineState::Win, "win")) return "invalid";
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
        for (char& c : cmd) c = static_cast<char>(std::tolower(c));

        if (cmd == "loadmap") return loadmap(arg);
        if (cmd == "validatemap") return validatemap();
        if (cmd == "addplayer") return addplayer(arg);
        if (cmd == "gamestart") return gamestart();

        if (cmd == "assignreinforcement") return assignreinforcement();
        if (cmd == "issueorder") return issueorder();
        if (cmd == "endissueorders") return endissueorders();
        if (cmd == "executeorder") return executeOrder();
        if (cmd == "endexecuteorder") return endExecuteOrder();

        if (cmd == "win") return win();
        if (cmd == "end") return end();

        std::cerr << "[Error] Unknown command: " << raw << "\n";
        return "invalid";
    }

    void GameEngine::clearGame() {
        
        delete deck; 
        deck = nullptr;

        for(Player* p : players){ delete p; }
        players.clear();

        delete gameMap;
        gameMap = nullptr;

        while (!playerQueue.empty()){playerQueue.pop();}
        currentPlayer = nullptr;
    }

} // namespace WarzoneEngine
