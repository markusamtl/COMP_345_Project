#include "GameEngine.h"
#include <sstream>
#include <algorithm>

namespace WarzoneEngine {

    /*------------------------------------------ENGINE STATE--------------------------------------------*/

    std::ostream& operator<<(std::ostream& os, const EngineState& s) {
        
        os << "Current State: ";

        switch (s) {

            case(EngineState::Start): os << "Start"; break;
            case(EngineState::MapLoaded): os << "Map Loaded"; break;
            case(EngineState::MapValidated): os << "Map Validated"; break;
            case(EngineState::PlayersAdded): os << "Players Added"; break;
            case(EngineState::AssignReinforcement): os << "Assignment Reinforcement"; break;
            case(EngineState::IssueOrders): os << "Issue Orders"; break;
            case(EngineState::ExecuteOrders): os << "Execute Orders"; break;
            case (EngineState::Win): os << "Win"; break;
            case (EngineState::End): os << "End"; break;
        
        }
        
        return os;
    
    }

    /*------------------------------------------GAME ENGINE--------------------------------------------*/

    //-- Constructors, Destructor, Copy Constructor, Assignment Operator, Stream Insertion Operator --//

    GameEngine::GameEngine() {

        state = EngineState::Start;
        gameMap = nullptr;
        deck = nullptr;
        players = {};
        playerQueue = {};
        currentPlayer = nullptr;

    }

    GameEngine::~GameEngine() { clearGame(); }

    GameEngine::GameEngine(const GameEngine& other) {

        //Copy all attributes
        state = other.state;
        gameMap = other.gameMap ? new Map(*other.gameMap) : nullptr;
        deck = other.deck ? new Deck(*other.deck) : nullptr;

        for(Player* p : other.players){ 
            
            if(p != nullptr){ players.push_back(new Player(*p)); }

        }

        queue<Player*> tempQueue = other.playerQueue; //Set up temp queue to copy from
        while (!tempQueue.empty()) {
            
            string name = tempQueue.front() -> getPlayerName(); //Peek at front of queue
            Player* found = findPlayerByName(name); //See if player exists in copied list
            
            if(found){ playerQueue.push(found); }
            
            tempQueue.pop(); //Remove from temp queue

        }

        currentPlayer = nullptr;
        if (other.currentPlayer){
         
            currentPlayer = findPlayerByName(other.currentPlayer -> getPlayerName());

        }

    }

    GameEngine& GameEngine::operator=(const GameEngine& other) {
        if (this != &other) {
            clearGame();
            state = other.state;

            gameMap = other.gameMap ? new Map(*other.gameMap) : nullptr;
            deck = other.deck ? new Deck(*other.deck) : nullptr;

            for (Player* p : other.players)
                players.push_back(new Player(*p));

            std::queue<Player*> tempQueue = other.playerQueue;
            while (!tempQueue.empty()) {
                std::string name = tempQueue.front()->getPlayerName();
                Player* found = findPlayerByName(name);
                if (found)
                    playerQueue.push(found);
                tempQueue.pop();
            }

            currentPlayer = nullptr;
            if (other.currentPlayer)
                currentPlayer = findPlayerByName(other.currentPlayer->getPlayerName());
        }
        return *this;
    }

    std::ostream& operator<<(std::ostream& os, const GameEngine& engine) {

        os << "GameEngine State: " << engine.state << "\n";
        os << "Players: " << engine.players.size() << "\n";
        os << "Map: " << (engine.gameMap ? "Loaded" : "None") << "\n";
        os << "Deck: " << (engine.deck ? "Initialized" : "None") << "\n";
        
        return os;
    
    }

    /*--------------------------------------Accessors & Mutators-------------------------------------------------*/

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

    /*--------------------------------------Helper & Internal Methods--------------------------------------------*/

    bool GameEngine::isCurrentStateCorrect(EngineState expected, const string& cmd) {
        
        if(state != expected) {
            
            cerr << "[Error] Command '" << cmd << "' invalid in state '" << state << "'.\n";
            return false;
        
        }
        
        return true;
    
    }

    void GameEngine::clearGame() {

        //Order of deletion matters here. Deck and players must be deleted before map, since they are independent of map.

        delete deck; //Deck can be deleted directly
        deck = nullptr;

        for(Player* p : players){ delete p; } //All players can be deleted directly
        players.clear();

        delete gameMap;
        gameMap = nullptr;

        while(!playerQueue.empty()){ playerQueue.pop(); } //Clear the queue
        currentPlayer = nullptr; //Previously deleted in players loop

    }

    void GameEngine::assignCountries() {

        // Get all territories from the map
        vector<Territory*> terrs = gameMap->getTerritories();
        if(terrs.empty()) { return; }

        if(players.empty()) { return; } // Avoid division by zero
        int terrPerPlayer = static_cast<int>(terrs.size() / (players.size() - 1)); // -1 to account for neutral player

        cout << "\n[Assignment] === Assigning Territories to Players ===\n";

        for(Player* p : players) {

            if (p == nullptr || p->getPlayerName() == "Neutral") { continue; } // Skip neutral player

            cout << "[Assignment] " << p -> getPlayerName() << " receives: ";

            for (int i = 0; i < terrPerPlayer && !terrs.empty(); i++) {

                // Pick random available territory
                size_t randomIndex = static_cast<size_t>(TimeUtil::getSystemTimeMillis() % terrs.size());
                Territory* selected = terrs[randomIndex];

                if(selected == nullptr){ continue; } //Skip null territories

                // Assign and deploy initial armies
                p -> addOwnedTerritories(selected);

                cout << selected -> getID(); //Print assigned territory
                if(i < terrPerPlayer - 1 && terrs.size() > 1) cout << ", "; //Manage commas

                terrs.erase(terrs.begin() + randomIndex); //Remove from available territories
            
            }

            cout << "\n";
        
        }

        // Assign leftover territories to the Neutral player
        Player* neutralPlayer = findPlayerByName("Neutral");
        if(neutralPlayer && !terrs.empty()) { //If neutral player exists and there are leftover territories

            cout << "[Assignment] Neutral player receives leftover territories: ";
            
            for(size_t i = 0; i < terrs.size(); i++) {
                
                Territory* t = terrs[i];
                if(t == nullptr){ continue; }

                neutralPlayer -> addOwnedTerritories(t);

                cout << t -> getID(); //Print assigned territory
                if (i < terrs.size() - 1) std::cout << ", "; //Manage commas
            }
            
            cout << "\n";
        
        }

        cout << "[Assignment] === Territory Assignment Complete ===\n";
    
    }

    bool GameEngine::startAgain() {

        if (state != EngineState::Win) { //Can only restart after game has been won
            cout << "[Warning] You can only restart the game when someone has won." << endl;
            return false;
        }

        string response;

        while (true) {

            cout << "\nThe game has ended. Would you like to start a new game? (yes, y / no, n): ";
            getline(std::cin, response);

            // Trim whitespace and convert to lowercase
            response.erase(remove_if(response.begin(), response.end(), ::isspace), response.end());
            transform(response.begin(), response.end(), response.begin(), ::tolower);

            clearGame(); //Clear all game data, since no matter the response, it must be reset

            if (response == "yes" || response == "y") {

                state = EngineState::Start;
                cout << "\n[GameEngine] The game has been reset. You may now load a new map and begin again.\n";
                return true;

            } else if (response == "no" || response == "n") {
              
                state = EngineState::End;
                cout << "\n[GameEngine] Exiting the game. See you next time!\n";
                return false;
            
            } else { cout << "\n[GameEngine] Invalid input. Please type 'yes' or 'no'."; }

        }

    }

    int GameEngine::computeReinforcementFor(Player* p) const {
        
        int ownedTerrs = static_cast<int>(p -> getOwnedTerritories().size());
        int baseArmies = max(3,  static_cast<int>(ownedTerrs / 3));
        int bonusArmies = 0;
        unordered_map<Continent*, long long> continentLookupTable = gameMap -> getContinentLookupTable();

        for(Continent* c : gameMap -> getContinents()) {
            
            if(c == nullptr){ continue; } //Skip null continents

            //Check if player controls entire continent, add bonus if so
            if(p -> controlsContinent(continentLookupTable, c)){ bonusArmies += c -> getBonusValue(); }

        }

        return baseArmies + bonusArmies;
    
    }

    void GameEngine::executeOrders(vector<WarzonePlayer::Player*>& players) {
        
        bool anyOrderLeft = true;
        
        while(anyOrderLeft) {
            
            anyOrderLeft = false;
            for(Player* p : players) {
                
                if(p == nullptr){ continue; } //Skip null players
                
                OrderList* list = p -> getPlayerOrders();
                if(list == nullptr){ continue; } //Skip players with no order list
                
                Order* firstOrder = list -> peek();

                if(firstOrder == nullptr){ continue; } //No orders left for this player
                anyOrderLeft = true;
                
                cout << "[" << p->getPlayerName() << "] executing: " << *firstOrder << std::endl;
                firstOrder -> execute();
                list->removeOrder(firstOrder);
            
            }

        }

    }

    Player* GameEngine::findPlayerByName(const string& name) const {
        
        for (Player* p : players) {
            
            if(p != nullptr && p -> getPlayerName() == name){ return p; }

        }

        return nullptr;
    
    }

    /*-------------------------------------------Game Commands---------------------------------------------------*/

    string GameEngine::loadmap(const string& path) {
        
        if(!isCurrentStateCorrect(EngineState::Start, "loadmap")){ return "Error: Current state is not loadmap!"; }
        clearGame();

        //Get map from MapLoader
        MapLoader loader;
        int loaderReturnCode = loader.importMapInfo(path);
        if (loaderReturnCode != 0) return "loadmap failed";

        pair<int, Map*> loadResults = loader.loadMap();
        int loaderCode = loadResults.first;
        gameMap = loadResults.second;
        if(loaderCode != 0 || gameMap == nullptr){ return "loadmap failed"; }

        state = EngineState::MapLoaded;
        return "Map has successfully loaded";

    }

    string GameEngine::validatemap() {

        if(!isCurrentStateCorrect(EngineState::MapLoaded, "validatemap")){ return "Error: Current state is not validatemap!"; }
        if(gameMap == nullptr){ return "No map to validate"; }

        if(!(gameMap -> validate())){ return "Invalid map"; }
        state = EngineState::MapValidated;
        return "Map has been sucessfully validated";

    }

    string GameEngine::addplayer(const string& name) {
        
        if(state != EngineState::MapValidated && state != EngineState::PlayersAdded) {
        
            return "Error: Not in a state to add players!";

        }

        if(name.empty()) { return "Error: Player name cannot be empty!"; }

        //Check if player with that name already exists
        if(findPlayerByName(name) != nullptr){ return "Error: Player with that name already exists!"; }

        addPlayer(name);
        state = EngineState::PlayersAdded;
        return "Player has been added";

    }

    string GameEngine::gamestart() {

        if(!isCurrentStateCorrect(EngineState::PlayersAdded, "gamestart")){ return "Error: Current state is not gamestart!"; }
        if(playerQueue.size() < 2){ return "Error: At least least 2 players are required to play"; }
        if(gameMap == nullptr){ return "Error: No map object was provided"; }

        //Always set the game to have a neutral player to handle order logic
        Player* neutral = new Player("Neutral", gameMap->buildEmptyContinentHashmap());
        players.push_back(neutral);
        cout << "[GameEngine] Added neutral player." << endl;

        assignCountries();

        if (deck == nullptr){

            deck = new Deck(static_cast<int>(players.size()) - 1); //-1 to account for neutral player

        } else { deck -> setNumOfPlayers(static_cast<int>(players.size()) - 1); } //Update deck to have correct number of players

        for (Player* p : players) { //Give each player 2 cards to start with

            if(p -> getPlayerName() == "Neutral"){ continue; } //Skip neutral player, they'll never get cards
            
            if(deck != nullptr && p -> getHand() != nullptr) { //Make sure deck and hand exist

                Card* c1 = deck -> draw();
                Card* c2 = deck -> draw();
                if(c1 != nullptr){ p -> getHand() -> addCardToHand(c1); }
                if(c2 != nullptr){ p -> getHand() -> addCardToHand(c2); }
            }

        }

        state = EngineState::AssignReinforcement;
        return "assign reinforcement";

    }

    string GameEngine::assignreinforcement() {

        if(!isCurrentStateCorrect(EngineState::AssignReinforcement, "assignreinforcement")) { 
            
            return "Error: Current state is not assignreinforcement!"; 
        
        }
        
        if(!gameMap) { return "Error: No map object was provided"; }

        cout << "\n[Reinforcement] === Assigning Reinforcements to Players ===\n";

        for (Player* p : players) {

            if (p == nullptr || p -> getPlayerName() == "Neutral") { continue; } // Skip null and neutral players

            int numArmiesToDeploy = computeReinforcementFor(p); //Calculate reinforcements
            if (numArmiesToDeploy <= 0) { continue; } //No reinforcements to deploy

            cout << "[Reinforcement] " << p -> getPlayerName() << " has " << numArmiesToDeploy << " armies to deploy.\n";

            int totNumTerrs = static_cast<int>(p -> getOwnedTerritories().size());
            
            if(totNumTerrs == 0) { //No territories owned, cannot deploy. This should never happen, but just in case.
                
                cout << "  (No owned territories — skipping)\n";
                continue;
            
            }

            int randomTerritoryIndex = 0;
            int randomArmiesDeployed = 0;

            // Track deployed armies for summary output
            unordered_map<string, int> deploymentSummary;

            while(numArmiesToDeploy > 0) {

                randomTerritoryIndex = static_cast<int>(TimeUtil::getSystemTimeNano() % totNumTerrs); //Pick random owned territory
                Territory* t = p -> getOwnedTerritories().getTerritories()[randomTerritoryIndex]; 
                if(t == nullptr){ continue; } //Skip null territories

                // Random number between 1 and numArmiesToDeploy
                randomArmiesDeployed = static_cast<int>(TimeUtil::getSystemTimeNano() % numArmiesToDeploy); //Get valid random number of armies 
                randomArmiesDeployed = max(1, randomArmiesDeployed); //Ensure at least 1 army is deployed

                t -> setNumArmies(t -> getNumArmies() + randomArmiesDeployed); //Deploy armies to territory
                numArmiesToDeploy -= randomArmiesDeployed;

                // Add to deployment summary
                deploymentSummary[t -> getID()] += randomArmiesDeployed;

            }

            // Print deployment breakdown for this player
            cout << "  Deployment Summary:\n";
            
            for(const auto& [territoryID, deployed] : deploymentSummary) { //Best way to iterate over a hashmap in C++17

                Territory* terr = gameMap -> getTerritoryByID(territoryID);
                if(terr != nullptr) {
                    
                    cout << "    - " << territoryID  << ": +" << deployed << " (Total: " << terr->getNumArmies() << " armies)\n";
                
                }
            
            }

            cout << "  => Reinforcements complete for " << p->getPlayerName() << ".\n\n";
        
        }

        cout << "[Reinforcement] === All Reinforcements Assigned ===\n";

        state = EngineState::IssueOrders;
        return "issue orders";

    }


    string GameEngine::issueorder() {

        if(!isCurrentStateCorrect(EngineState::IssueOrders, "issueorder")){ return "Error: Current state is not issueorder!"; }

        for (Player* p : players) {
        
            if(p == nullptr || p -> getPlayerName() == "Neutral"){ continue; } //Skip null players and neutral player
            p -> issueOrder(); //Randomly issue an order

            //Add random logic to create a real order. Will help to simulate gameplay for now.
            Order* issuedOrder = p -> getPlayerOrders() -> peek();
            
            //Get random numbers to help populate order info
            long long randomNumberOne = TimeUtil::getSystemTimeNano();
            long long randomNumberTwo = TimeUtil::getSystemTimeNano();
            long long randomNumberThree = TimeUtil::getSystemTimeNano();

            switch(issuedOrder -> getOrderType()) {
                    
                case (OrderType::Advance): { // Player advances to a random territory

                    vector<Territory*> candidateSources = p -> getSourcesWithManyArmies(); // Only territories with >1 army
                    if (candidateSources.empty()) {cout << "bp1"; break; } // No valid source territories

                    Territory* sourceTerr = candidateSources[randomNumberOne % candidateSources.size()]; // Pick one randomly
                    if(sourceTerr == nullptr) { cout << "bp2";break; }

                    vector<Territory*> neighbors = sourceTerr->getNeighbors();
                    if(neighbors.empty()) { cout << "bp3"; break; } // No neighbors, cannot advance

                    Territory* targetTerr = neighbors[randomNumberTwo % neighbors.size()]; // Random neighbor
                    if(targetTerr == nullptr) { cout << "bp4";break; }

                    Player* targetPlayer = targetTerr -> getOwner();
                    if(targetPlayer == nullptr) { cout << "bp5";break; } // Should not happen

                    int armiesAvailable = sourceTerr -> getNumArmies();
                    int numArmiesToMove = 0;

                    if(armiesAvailable == 2) { numArmiesToMove = 1; }
                    // Move at least 1, leave 1 behind
                    else { numArmiesToMove = static_cast<int>(1 + (randomNumberThree % static_cast<long long>(armiesAvailable - 1))); }
                    
                    if(numArmiesToMove <= 0 || numArmiesToMove >= armiesAvailable) { 
                       cout << "bp6"; break; 
                    }

                    Advance* advanceOrder = new Advance(p, sourceTerr, targetTerr, numArmiesToMove);
                    if(advanceOrder != nullptr) {

                        OrderList* playerOrders = p->getPlayerOrders();
                        if(playerOrders == nullptr) { break; }

                        playerOrders->replaceOrder(issuedOrder, advanceOrder);
                    }

                } break;


                case(OrderType::Bomb): { //Player bombs a random enemy territory

                    // Collect candidate target territories (owned by another player)
                    vector<Territory*> candidateTargets = p -> getBombCandidates();

                    if (candidateTargets.empty()) { break; } // No valid bombing targets — skip this order
                
                    Territory* randomTarget = candidateTargets[randomNumberOne % candidateTargets.size()]; //Random target territory
                    if(randomTarget == nullptr){ break; } //Should never happen, but just in case

                    Bomb* bombOrder = new Bomb(p, randomTarget);

                    if(bombOrder != nullptr){

                        OrderList* playerOrders = p -> getPlayerOrders();
                        if(playerOrders == nullptr){ break; } //Should never happen, but just in case

                        playerOrders -> replaceOrder(issuedOrder, bombOrder); //Replace the empty order with a real one

                    }

                } break;

                case(OrderType::Blockade): { //Player blockades a random owned territory

                    vector<Territory*> candidateTargets = p -> getOwnedTerritories().getTerritories();
                    if(candidateTargets.empty()){ break; } //Should never happen, but just in case

                    Territory* randomTarget = candidateTargets[randomNumberOne % candidateTargets.size()]; //Random target territory
                    if(randomTarget == nullptr){ break; } //Should never happen, but just in case
                    Blockade* blockadeOrder = new Blockade(p, randomTarget, findPlayerByName("Neutral"));

                    if(blockadeOrder != nullptr){

                        OrderList* playerOrders = p -> getPlayerOrders();
                        if(playerOrders == nullptr){ break; } //Should never happen, but just in case

                        playerOrders -> replaceOrder(issuedOrder, blockadeOrder); //Replace the empty order with a real one

                    }

                } break;
                
                case(OrderType::Airlift): { //Player airlifts between two random owned territories

                    PlayerTerrContainer PlayerContainer(p -> getOwnedTerritories()); //Get a copy of the player's territory container
                    vector<Territory*> terrCopy = PlayerContainer.getTerritories(); //Extract the territories from the copied container
                    if(terrCopy.size() < 2){ break; } //Need at least 2 territories to airlift

                    vector<Territory*> candidateSources = p -> getSourcesWithManyArmies(); // Only territories with >1 army
                    if (candidateSources.empty()) { break; } // No valid source territories

                    Territory* sourceTerr = candidateSources[randomNumberOne % candidateSources.size()]; //Random source territory
                    
                    terrCopy.erase(std::remove(terrCopy.begin(), terrCopy.end(), sourceTerr), terrCopy.end()); //Remove source from total territories

                    Territory* targetTerr = (terrCopy)[randomNumberTwo % terrCopy.size()]; //Random target territory

                    //At least 1 army, at most all armies - 1 (need to leave at least 1 behind)
                    long long randNumArmiesToMove = randomNumberThree % (sourceTerr -> getNumArmies() - 1);
                    int numArmiesToMove = max(1, static_cast<int>(randNumArmiesToMove));
                    if(numArmiesToMove <= 0){ break; } //Should never happen, but just in case

                    Airlift* airliftOrder = new Airlift(p, sourceTerr, targetTerr, numArmiesToMove);

                    if(airliftOrder != nullptr){

                        OrderList* playerOrders = p -> getPlayerOrders();
                        if(playerOrders == nullptr){ break; } //Should never happen, but just in case

                        playerOrders -> replaceOrder(issuedOrder, airliftOrder); //Replace the empty order with a real one

                    }

                } break;
                
                case(OrderType::Negotiate): { //Player negotiates with a random player

                    vector<Player*> potentialTargets;
                    for(Player* other : players) {
                        
                        //Skip null players, neutral player, and self
                        if(other == nullptr || other -> getPlayerName() == "Neutral" || other -> getPlayerName() == p -> getPlayerName()){ continue; }
                        potentialTargets.push_back(other);
                    
                    }

                    if(potentialTargets.empty()){ break; } //No valid targets, cannot negotiate

                    Player* targetPlayer = potentialTargets[randomNumberOne % potentialTargets.size()]; //Random target player
                    if(targetPlayer == nullptr){ break; } //Should never happen, but just in case

                    Negotiate* negotiateOrder = new Negotiate(p, targetPlayer);

                    if(negotiateOrder != nullptr){

                        OrderList* playerOrders = p -> getPlayerOrders();
                        if(playerOrders == nullptr){ break; } //Should never happen, but just in case

                        playerOrders -> replaceOrder(issuedOrder, negotiateOrder); //Replace the empty order with a real one

                    }

                } break;
                default: break; //Do nothing for other order types
            }
        
        }

        return "Orders have been issued";

    }

    string GameEngine::endissueorders() {
        
        if(!isCurrentStateCorrect(EngineState::IssueOrders, "endissueorders")){ return "Error: Current state is not endissueorders!"; }
        state = EngineState::ExecuteOrders;
        return "Moving on to execute orders";

    }

    string GameEngine::executeOrder() {

        if(state != EngineState::ExecuteOrders){ return "Invalid state transition: must be in ExecuteOrders."; }

        // ---------------------------- Setup ----------------------------
        vector<Player*> ordered;
        queue<Player*> tempQueue = playerQueue;

        while (!tempQueue.empty()){ // Transfer to vector for easier iteration
            
            ordered.push_back(tempQueue.front());
            tempQueue.pop();

        }

        // Vector to collect players that must be deleted later
        vector<Player*> toDelete;

        // ---------------------------- Execute Orders ----------------------------
        for(Player* p : ordered) {

            if(p == nullptr || p -> getPlayerName() == "Neutral"){ continue; } //Skip null players and neutral player

            cout << "[" << p -> getPlayerName() << "] executing orders..." << endl;

            OrderList* orders = p -> getPlayerOrders();

            if(!orders || orders->size() == 0) { //No orders to execute
                cout << "No orders to execute for " << p -> getPlayerName() << endl;
                continue;
            }

            while(orders -> size() > 0) { //Execute all orders for this player
                
                Order* o = orders-> peek(); //Get the first order
                
                if(!(o -> validate())) { //Order is invalid
                    
                    cout << "[" << p -> getPlayerName() << "] Invalid order: " << *o << ". Skipping execution." << endl;
                    orders -> removeOrder(0); //Remove invalid order
                    continue; //Skip to next order
                
                }

                if(o != nullptr) {
                    
                    cout << "[" << p -> getPlayerName() << "] Executes: " << *o << endl;
                    o -> execute();
                    orders -> removeOrder(0); //Remove executed order
                
                }

            }

            cout << "[" << p -> getPlayerName() << "] has finished executing orders." << endl;
        
        }

        // ---------------------------- Clearing Truces -----------------------------
        for(Player* p: players){

            p -> clearNeutralEnemies(); //Clear out neutral enemy list at the end of every turn

        }

        // ---------------------------- Eliminate Players ----------------------------
        for(auto it = players.begin(); it != players.end();) { //Use an iterator to allow safe removal

            Player* p = *it; //Dereference iterator to get player

            if(p == nullptr || p -> getPlayerName() == "Neutral") { //Make SURE that neutral cant be eliminated
                it++;
                continue;
            }

            int terrCount = static_cast<int>(p->getOwnedTerritories().size());
            if(terrCount == 0) {

                cout << "[" << p -> getPlayerName() << "] has been eliminated from the game." << endl;

                // Remove from playerQueue
                queue<Player*> newQueue;

                while(!playerQueue.empty()) {

                    Player* front = playerQueue.front(); //Peek at front of queue
                    playerQueue.pop(); //Remove from old queue
                    if(front != p){ newQueue.push(front); } //Only add to new queue if not the eliminated player
                
                }

                playerQueue = newQueue;

                // Mark player for deletion AFTER this method finishes
                toDelete.push_back(p);
                it = players.erase(it); //Remove from players list and get new iterator
                continue;
            }

            it++;

        }

        // ---------------------------- Win Condition Check ----------------------------

        int activePlayers = 0;
        Player* potentialWinner = nullptr;
        bool hasWon = false;
        bool controlsMap = false;

        //First Check: If there's only 1 non-neutral player on the map
        for(Player* p : players) { 

            //Skip null and neutral players
            if(p == nullptr){ continue; }
            if(p -> getPlayerName() == "Neutral"){ continue; }

            int terrCount = static_cast<int>(p -> getOwnedTerritories().size()); //Get size of territories that player owns
            if (terrCount > 0) { //See if a player has more than 1 territories
                
                activePlayers++;
                potentialWinner = p;

            }

        }

        //Set hasWon flag true
        if(activePlayers == 1 && potentialWinner != nullptr) { 
            
            hasWon = true;
            currentPlayer = potentialWinner;

        }

        //Second Check: See if the player who won controls the entire map
        const unordered_map<Continent*, long long>& refTable = gameMap -> getContinentLookupTable(); //Get reference table from map

        for(Player* p : ordered) {

            // Skip eliminated or null players
            if(p == nullptr){ continue; }
            
            if(find(toDelete.begin(), toDelete.end(), p) != toDelete.end()){ continue; }//Skip players marked for deletion

            if(p -> controlsMap(refTable)) {

                currentPlayer = p;
                controlsMap = true;
                break;

            }
        
        }

        // ---------------------------- Deferred Cleanup ----------------------------
        for(Player* dead : toDelete) {
            
            if(dead != nullptr) {

                cout << "[Cleanup] Deleting player object for " << dead -> getPlayerName() << endl;
                delete dead;

            }

        }

        toDelete.clear();

        // ---------------------------- Phase Transition ----------------------------

        if(hasWon && controlsMap) {

            state = EngineState::Win;

            // Log the winner
            cout << "\n=== WINNER DETECTED ===" << endl;
            cout << "[" << currentPlayer -> getPlayerName() << "] controls all continents!" << endl;
            return "win";       

        } else if(hasWon) {

            state = EngineState::Win;

            // Log the winner
            cout << "\n=== WINNER DETECTED ===" << endl;
            cout << "[" << currentPlayer -> getPlayerName() << "] has eliminated all enemy players!" << endl;
            return "win";  

        }

        state = EngineState::AssignReinforcement;
        cout << "Assignment Reinforcement phase has been completed, moving on to next phase" << endl;
        return "endexecuteorder";

    }

    string GameEngine::endExecuteOrder() {

        if(!isCurrentStateCorrect(EngineState::ExecuteOrders, "endexecuteorder")){ return "Error: Current state is not endexecuteorder!"; }
        state = EngineState::AssignReinforcement;
        return "assign reinforcement";
    
    }

    string GameEngine::win() {

        if (!isCurrentStateCorrect(EngineState::Win, "win")){ return "Error: Current state is not win!"; }
        cout << "[" << currentPlayer -> getPlayerName() << "] has won the game!" << endl;
        
        bool startNewGame = startAgain();

        if(startNewGame) { 

            state = EngineState::Start;
            return "Starting new game";

        } else {

            state = EngineState::End;
            return "Ending program";

        }
    
    }

    string GameEngine::end() {

        return "The program has now terminated. Thank you for playing!";

    }

    /*-----------------------------------------Player Queue Management-------------------------------------------*/

    void GameEngine::addPlayer(const std::string& playerName) {

        Player* newPlayer = new Player(playerName, gameMap -> buildEmptyContinentHashmap());
        players.push_back(newPlayer);
        playerQueue.push(newPlayer);

    }

    void GameEngine::addPlayers(int count) {
        
        for(int i = 1; i <= count; i++) { addPlayer("Player " + std::to_string(i)); }

    }

    Player* GameEngine::getNextPlayer() {

        return playerQueue.empty() ? nullptr : playerQueue.front();

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

    bool GameEngine::hasPlayers() const { return !playerQueue.empty(); }

    /*-------------------------------------------Command Dispatcher----------------------------------------------*/

    string GameEngine::processCommand(const string& raw, const string& arg) {
        string cmd = raw;
        for (char& c : cmd) c = static_cast<char>(tolower(c));

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

}