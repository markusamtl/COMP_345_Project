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
    // Copy Constructor
    GameEngine::GameEngine(const GameEngine& other) {
        // Copy primitive/state members
        state = other.state;

        // Deep copy map
        gameMap = other.gameMap ? new Map(*other.gameMap) : nullptr;

        // Deep copy deck
        deck = other.deck ? new Deck(*other.deck) : nullptr;

        // Deep copy players
        for (Player* p : other.players) {
            players.push_back(new Player(*p));
        }

        // Copy player queue (by matching player names to newly copied players)
        std::queue<Player*> tempQueue = other.playerQueue;
        while (!tempQueue.empty()) {
            std::string name = tempQueue.front()->getPlayerName();
            auto it = std::find_if(players.begin(), players.end(),
                                   [&](Player* pp) { return pp->getPlayerName() == name; });
            if (it != players.end())
                playerQueue.push(*it);
            tempQueue.pop();
        }

        // Copy current player pointer
        currentPlayer = nullptr;
        if (other.currentPlayer) {
            auto it = std::find_if(players.begin(), players.end(),
                                   [&](Player* p) { return p->getPlayerName() == other.currentPlayer->getPlayerName(); });
            if (it != players.end())
                currentPlayer = *it;
        }
    } 

    //TODO: Implement assignment operator
    // Assignment Operator
    GameEngine& GameEngine::operator=(const GameEngine& other) {
        if (this != &other) {
            clearGame(); // Free existing resources

            state = other.state;

            // Deep copy map
            gameMap = other.gameMap ? new Map(*other.gameMap) : nullptr;

            // Deep copy deck
            deck = other.deck ? new Deck(*other.deck) : nullptr;

            // Deep copy players
            for (Player* p : other.players) {
                players.push_back(new Player(*p));
            }

            // Copy player queue
            std::queue<Player*> tempQueue = other.playerQueue;
            while (!tempQueue.empty()) {
                std::string name = tempQueue.front()->getPlayerName();
                auto it = std::find_if(players.begin(), players.end(),
                                       [&](Player* pp) { return pp->getPlayerName() == name; });
                if (it != players.end())
                    playerQueue.push(*it);
                tempQueue.pop();
            }

            // Copy current player pointer
            currentPlayer = nullptr;
            if (other.currentPlayer) {
                auto it = std::find_if(players.begin(), players.end(),
                                       [&](Player* p) { return p->getPlayerName() == other.currentPlayer->getPlayerName(); });
                if (it != players.end())
                    currentPlayer = *it;
            }
        }
        return *this;
    }
    

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

    Map* GameEngine::getGameMap() const { return gameMap; }
    void GameEngine::setGameMap(Map* map) { gameMap = map; }

    Deck* GameEngine::getDeck() const { return deck; }
    void GameEngine::setDeck(Deck* d) { deck = d; }

    const std::vector<Player*>& GameEngine::getPlayers() const { return players; }
    void GameEngine::setPlayers(const std::vector<Player*>& newPlayers) { players = newPlayers; }

    std::queue<Player*> GameEngine::getPlayerQueue() const { return playerQueue; }
    void GameEngine::setPlayerQueue(const std::queue<Player*>& q) { playerQueue = q; }

    Player* GameEngine::getCurrentPlayer() const { return currentPlayer; }
    void GameEngine::setCurrentPlayer(Player* player) { currentPlayer = player; }


    //-- Class Methods --//

    // ------------------ TRANSITIONS ------------------

    bool GameEngine::isCurrentStateCorrect(EngineState expected, const string& cmd) {

        if(state != expected) {

            std::cerr << "[Error] Command '" << cmd << "' invalid in state '" << state << "'.\n";
        
            return false;
        }

        return true;
    
    }

    /**
     * @brief Loads and initializes the game map from a given file path.
     * @param path The path to the .map file.
     * @return A message indicating success or failure.
     */
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

    /**
     * @brief Validates the map for connectivity and correct continent structure.
     * @return Status message indicating whether the map is valid.
     */
    string GameEngine::validatemap() {

        if(!isCurrentStateCorrect(EngineState::MapLoaded, "validatemap")){ return "validatemap is not a valid state!"; }
        if (!gameMap) return "no map";

        bool good = gameMap->validate();
        if (!good) return "map invalid";

        state = EngineState::MapValidated;
        return "map validated";
    }

    // ------------------ Player queue management ------------------

    /**
     * @brief Creates and registers a new player in the game.
     * @param playerName The name of the player to add.
     */
    void GameEngine::addPlayer(const std::string& playerName) {
        Player* newPlayer = new Player(playerName,
            std::unordered_map<WarzoneMap::Continent*, long long>{});

        players.push_back(newPlayer);
        playerQueue.push(newPlayer);

        std::cout << "Player added: " << playerName << std::endl;
    }

    /**
     * @brief Quickly adds a specified number of players for testing.
     * @param count Number of players to add (default: 3).
     */
    void GameEngine::addPlayers(int count) {
        for (int i = 1; i <= count; ++i) {
            std::string name = "Player" + std::to_string(i);
            addPlayer(name);
        }
        std::cout << count << " players added successfully.\n";
    }

    /**
     * @brief Retrieves the next player in the queue.
     * @return Pointer to the next player or nullptr if queue is empty.
     */
    Player* GameEngine::getNextPlayer() {
        if (!playerQueue.empty())
            return playerQueue.front();
        return nullptr;
    }

    /**
     * @brief Advances the player queue to the next player's turn.
     */
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

    /**
     * @brief Checks if there are active players in the game.
     * @return True if the player queue is not empty.
     */
    bool GameEngine::hasPlayers() const {
        return !playerQueue.empty();
    }

    // ------------------ core game flow ------------------

    /**
     * @brief Adds a player during setup phase.
     * @param name Player name.
     * @return Status message.
     */
    string GameEngine::addplayer(const string& name) {
        if (state != EngineState::MapValidated && state != EngineState::PlayersAdded) {
            std::cerr << "[Error] 'addplayer' invalid in state '" << state << "'.\n";
            return "invalid";
        }

        addPlayer(name);
        state = EngineState::PlayersAdded;
        return "players added";
    }

    /**
     * @brief Initializes the main game loop after setup.
     * @return Status message indicating the next phase.
     */
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

    /**
     * @brief Randomly assigns map territories to players.
     */
    void GameEngine::assignCountries() {
        vector<Territory*> terrs = gameMap->getTerritories();
        if (terrs.empty()) return;

        // Copy queue order to vector for assignment
        vector<Player*> ordered;
        queue<Player*> copyQueue = playerQueue;

        while (!copyQueue.empty()) {
            ordered.push_back(copyQueue.front());
            copyQueue.pop();
        }

        // Round-robin territory assignment
        size_t pi = 0;
        for (Territory* t : terrs) {
            ordered[pi]->addOwnedTerritories(t);
            pi = (pi + 1) % ordered.size();
        }

        // Ensure all territories have at least 1 army
        for (Player* p : ordered) {
            for (Territory* t : p->getOwnedTerritories().getTerritories())
                if (t) t->setNumArmies(std::max(1, t->getNumArmies()));
        }
    }

    /**
     * @brief Computes the total reinforcement armies for a player.
     */
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

    /**
     * @brief Assigns reinforcement armies to all players based on territories and continents owned.
     */
    string GameEngine::assignreinforcement() {
         if (!isCurrentStateCorrect(EngineState::AssignReinforcement, "assignreinforcement")) 
            return "invalid";
         if (!gameMap) return "no map";

         vector<Player*> ordered;
         auto copyQueue = playerQueue;
         while (!copyQueue.empty()) {
             ordered.push_back(copyQueue.front());
             copyQueue.pop();
    }

    // Compute and store reinforcements in GameEngine's map
    for (Player* p : ordered) {
        if (!p) continue;
        
        int r = computeReinforcementFor(p);
        
        // Store in GameEngine's reinforcement tracking map
        reinforcementPools[p] = reinforcementPools[p] + r;
        
        std::cout << "[Reinforcement] " << p->getPlayerName()
                  << " receives " << r << " armies. "
                  << "Total pool: " << reinforcementPools[p] << "\n";
    }

    state = EngineState::IssueOrders;
    return "issue orders";
}

    /**
     * @brief Invokes the issueOrder() method for each player.
     */
    string GameEngine::issueorder() {
        if (!isCurrentStateCorrect(EngineState::IssueOrders, "issueorder")) return "invalid";
        for (auto& p : players) p->issueOrder();
        return "issuing orders";
    }

    /**
     * @brief Ends the IssueOrders phase and transitions to ExecuteOrders.
     */
    string GameEngine::endissueorders() {
        if (!isCurrentStateCorrect(EngineState::IssueOrders, "endissueorders")) return "invalid";
        state = EngineState::ExecuteOrders;
        return "execute orders";
    }

    // ------------------ interleaved executeOrder ------------------

    /**
     * @brief Executes player orders in an interleaved manner.
     */
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

    /**
     * @brief Helper function that executes orders interleaved between players.
     */
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

    /**
     * @brief Ends ExecuteOrders phase and transitions back to reinforcement.
     */
    string GameEngine::endExecuteOrder() {
        if (!isCurrentStateCorrect(EngineState::ExecuteOrders, "endexecuteorder")) return "invalid";
        state = EngineState::AssignReinforcement;
        return "assign reinforcement";
    }

    /**
     * @brief Outputs a win state message.
     */
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

    /**
     * @brief Processes text-based commands and dispatches them to corresponding methods.
     * @param raw The command string (case-insensitive).
     * @param arg Optional argument (e.g., map path or player name).
     * @return Response message from the executed command.
     */
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
