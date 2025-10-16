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
        turn = 1;
        maxTurns = INT_MAX;
        playerQueue = {};
        currentPlayer = nullptr;

    }

    GameEngine::~GameEngine() { clearGame(); }

    GameEngine::GameEngine(const GameEngine& other) {

        //Copy all attributes
        state = other.state;
        gameMap = other.gameMap ? new Map(*other.gameMap) : nullptr;
        deck = other.deck ? new Deck(*other.deck) : nullptr;
        turn = other.turn;
        maxTurns = other.maxTurns;

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

            //Clone easily clonable datatypes
            gameMap = other.gameMap ? new Map(*other.gameMap) : nullptr;
            deck = other.deck ? new Deck(*other.deck) : nullptr;
            turn = other.turn;
            maxTurns = other.maxTurns;

            //Clone Players
            for(Player* p : other.players){
                
                players.push_back(new Player(*p));

            }

            //Clone Player Queue
            queue<Player*> tempQueue = other.playerQueue;
            while (!tempQueue.empty()) {
                
                string name = tempQueue.front() -> getPlayerName();
                Player* found = findPlayerByName(name);
                if(found){ playerQueue.push(found); }
                tempQueue.pop();

            }

            currentPlayer = nullptr;
            if (other.currentPlayer){ currentPlayer = findPlayerByName(other.currentPlayer->getPlayerName()); }

        }

        return *this;

    }

    std::ostream& operator<<(std::ostream& os, const GameEngine& engine) {

        os << "GameEngine State: " << engine.state << "\n";
        os << "Number of Players: " << engine.players.size() << "\n";
        os << "Map Status: " << (engine.gameMap ? "Loaded" : "None") << "\n";
        os << "Card Deck Status: " << (engine.deck ? "Initialized" : "None") << "\n";
        os << "Current Turn: " << engine.turn;
        os << "Maximum number of Turns:" << engine.maxTurns;
        
        return os;
    
    }

    /*--------------------------------------Accessors & Mutators-------------------------------------------------*/

    Map* GameEngine::getGameMap() const { return gameMap; }
    void GameEngine::setGameMap(Map* map) { gameMap = map; }

    Deck* GameEngine::getDeck() const { return deck; }
    void GameEngine::setDeck(Deck* d) { deck = d; }

    const vector<Player*>& GameEngine::getPlayers() const { return players; }
    void GameEngine::setPlayers(const std::vector<Player*>& newPlayers) { players = newPlayers; }

    int GameEngine::getTurn() const { return turn; }
    void GameEngine::setTurn(int turn){ this -> turn = turn; }

    int GameEngine::getMaxTurns() const { return maxTurns; }
    void GameEngine::setMaxTurns(int maxTurns){ this -> maxTurns = maxTurns; }

    queue<Player*> GameEngine::getPlayerQueue() const { return playerQueue; }
    void GameEngine::setPlayerQueue(const queue<Player*>& q) { playerQueue = q; }

    Player* GameEngine::getCurrentPlayer() const { return currentPlayer; }
    void GameEngine::setCurrentPlayer(Player* player) { currentPlayer = player; }

    const EngineState& GameEngine::getState() const { return state; }

    const string GameEngine::getStateAsString() const {

        switch(state) {

            case EngineState::Start:
                return "Start";

            case EngineState::MapLoaded:
                return "Map Loaded";

            case EngineState::MapValidated:
                return "Map Validated";

            case EngineState::PlayersAdded:
                return "Players Addition";

            case EngineState::AssignReinforcement:
                return "Assignment Reinforcement";

            case EngineState::IssueOrders:
                return "Issue Orders";

            case EngineState::ExecuteOrders:
                return "Execute Orders";

            case EngineState::Win:
                return "Win";

            case EngineState::End:
                return "End";

            default:
                return "Unknown State";
        }
    }

    void GameEngine::setState(const EngineState& gameState){ state = gameState;}

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

    string GameEngine::shufflePlayers(bool surpressOutput) {

        if (playerQueue.size() <= 2) { return "Not enough players to shuffle.\n"; }

        ostringstream output;
        if(surpressOutput == false) {

            output << "\n === Randomizing Player Queue ===\n";
            output << " Original queue order: ";

            //Make a copy to print queue without modifying original
            queue<Player*> tempDisplay = playerQueue;
            while(!tempDisplay.empty()) {

                Player* p = tempDisplay.front(); 
                tempDisplay.pop();

                if(p != nullptr){ output << p -> getPlayerName(); }
                if(!tempDisplay.empty()){ output << ", "; }

            }

            output << "\n\n";

        }

        // === Extract all non-neutral players from queue ===
        vector<Player*> tempPlayers;
        while(!playerQueue.empty()) {

            Player* curr = playerQueue.front(); 
            playerQueue.pop();

            if(curr == nullptr) { continue; }

            //Skip the neutral player so it never enters the shuffled rotation
            if(curr -> getPlayerName() == "Neutral") { continue; }

            tempPlayers.push_back(curr);
        }

        if(tempPlayers.size() <= 1) { return "[Shuffle] Not enough non-neutral players to shuffle.\n"; }

        // Generate random entropy per player
        vector<pair<long long, Player*>> shuffleOrder;
        shuffleOrder.reserve(tempPlayers.size());

        //Generate highly entropic pseudorandom values for each player
        for(Player* p : tempPlayers) {

            unsigned long long randomValue = static_cast<unsigned long long>(TimeUtil::getSystemTimeNano());

            //Take bitwise xor from randomValue and randomValue / 2^21, 2^21 = 2097152 
            //Multiply by Knuth's constant for 64 bits (2^64 * Golden ratio) to introduce randomness 
            //Automatically reduced by mod 2^64 because of long long limit 
            //XOR introduces instability, as it flips bits wherever the high and low halves disagree
            randomValue = (randomValue ^ (randomValue >> 21)) * 11400714819323198485ULL;

            shuffleOrder.emplace_back(static_cast<long long>(randomValue), p);

        }

        //Sort randomly generated values (Insertion Sort)
        for(size_t i = 1; i < shuffleOrder.size(); i++) {

            pair<long long, Player*> key = shuffleOrder[i];
            size_t j = i;
            while(j > 0 && shuffleOrder[j - 1].first > key.first) {

                shuffleOrder[j] = shuffleOrder[j - 1];
                j--;

            }

            shuffleOrder[j] = key;

        }

        //Push shuffled players back into the queue 
        for(pair<long long, Player*> entry : shuffleOrder) {

            playerQueue.push(entry.second);

        }

        //Print new shuffled order
        if(surpressOutput == false) {

            output << "[Shuffle] New randomized queue order: ";

            queue<Player*> tempDisplay = playerQueue;
            while(!tempDisplay.empty()) {

                Player* p = tempDisplay.front(); 
                tempDisplay.pop();

                if(p != nullptr){ output << p -> getPlayerName(); }
                if(!tempDisplay.empty()){ output << ", "; }

            }

            output << "\n[Shuffle] === Player Queue Shuffle Complete ===\n";

        }

        return surpressOutput ? "" : output.str();

    }

    string GameEngine::assignTerritories(bool surpressOutput) {

        // Get all territories from the map
        vector<Territory*> terrs = gameMap -> getTerritories();
        if(terrs.empty()){ return "[Assignment] No territories available for assignment.\n"; }

        if(players.empty()){ return "[Assignment] No players available for assignment.\n"; } // Avoid division by zero
        int terrPerPlayer = static_cast<int>(terrs.size() / (players.size() - 1)); // -1 to account for neutral player

        ostringstream output;
        if(surpressOutput == false){ output << "\n[Assignment] === Assigning Territories to Players ===\n"; }

        for(Player* p : players) {

            if(p == nullptr || p->getPlayerName() == "Neutral") { continue; } // Skip neutral player

            if(surpressOutput == false){ output << "[Assignment] " << p -> getPlayerName() << " receives: "; }

            for(int i = 0; i < terrPerPlayer && !terrs.empty(); i++) {

                // Pick random available territory
                size_t randomIndex = static_cast<size_t>(TimeUtil::getSystemTimeMillis() % terrs.size());
                Territory* selected = terrs[randomIndex];

                if(selected == nullptr){ continue; } //Skip null territories

                // Assign and deploy initial armies
                p -> addOwnedTerritories(selected);

                if(surpressOutput == false){

                    output << selected -> getID(); //Print assigned territory
                    if(i < terrPerPlayer - 1 && terrs.size() > 1) output << ", "; //Manage commas

                }

                terrs.erase(terrs.begin() + randomIndex); //Remove from available territories
            
            }

            if(surpressOutput == false){ output << "\n"; }
        
        }

        // Assign leftover territories to the Neutral player
        Player* neutralPlayer = findPlayerByName("Neutral");
        if(neutralPlayer && !terrs.empty()) { //If neutral player exists and there are leftover territories

            if(surpressOutput == false){ output << "[Assignment] Neutral player receives leftover territories: "; }
            
            for(size_t i = 0; i < terrs.size(); i++) {
                
                Territory* t = terrs[i];
                if(t == nullptr){ continue; }

                neutralPlayer -> addOwnedTerritories(t);

                if(surpressOutput == false){

                    output << t -> getID(); //Print assigned territory
                    if (i < terrs.size() - 1) output << ", "; //Manage commas

                }

            }
            
            if(surpressOutput == false){ output << "\n\n"; }
        
        }

        if(surpressOutput == false){ output << "[Assignment] === Territory Assignment Complete ===\n\n"; }

        return surpressOutput ? "" : output.str();
    
    }

    void GameEngine::handleBombOrder(Player* p, Player* neutralPlayer, Order* issuedOrder, bool surpressOutput, ostringstream& output) {
        
        //Player bombs the strongest enemy, not under truce. Avoid Neutral player if possible
        vector<Territory*> candidateTargets = p -> getBombCandidates();

        if(candidateTargets.empty()){ 
            
            issuedOrder -> setEffect("[IssueOrder] Player: " + p -> getPlayerName() +
                                        " can't issue a bomb order, as they have no valid candidates to bomb\n");

            if(!surpressOutput){

                output << "[IssueOrder] Player: " << p -> getPlayerName() 
                    << " can't issue a bomb order, as they have no valid candidates to bomb\n";
            
            }
            
            return; 
        
        }

        //Separate neutral and non-neutral targets
        vector<Territory*> nonNeutralTargets;
        vector<Territory*> neutralTargets;

        //ONLY ATTACK NEUTRAL CANDIDATE IF THERE'S NO OTHER CHOICE TO 
        for(Territory* t : candidateTargets) {

            if(t == nullptr){ continue; }

            Player* targetOwner = t -> getOwner();
            if(targetOwner == nullptr || targetOwner == p){ continue; }

            if(targetOwner == neutralPlayer) { neutralTargets.push_back(t); } 
            else { nonNeutralTargets.push_back(t); }

        }

        //Prefer non-neutral targets if possible
        vector<Territory*> validTargets = nonNeutralTargets.empty() ? neutralTargets : nonNeutralTargets;

        if(validTargets.empty()){

            issuedOrder -> setEffect("[IssueOrder] Player: " + p -> getPlayerName() +
                                        " can't issue a bomb order, as they have no valid targets to bomb\n");

            if(!surpressOutput){ 

                output << "[IssueOrder] Player: " << p -> getPlayerName() 
                    << " can't issue a bomb order, as they have no valid targets to bomb\n";
            
            }

            return;

        }

        sort(validTargets.begin(), validTargets.end(), Territory::territoryNumArmiesCompareDescend);

        Territory* chosenTarget = validTargets.front();
        if(chosenTarget == nullptr){ 

            issuedOrder -> setEffect("[IssueOrder] Player: " + p -> getPlayerName() 
                                    + " can't issue a bomb order, as the target they have chosen is invalid!\n");
            if(!surpressOutput){ 
                output << "[IssueOrder] Player: " << p -> getPlayerName() 
                    << " can't issue a bomb order, as the target they have chosen is invalid!\n";
            }
            return; 

        }

        Bomb* bombOrder = new Bomb(p, chosenTarget);
        if(bombOrder != nullptr){
            OrderList* playerOrders = p -> getPlayerOrders();
            if(playerOrders == nullptr){ return; }

            playerOrders -> replaceOrder(issuedOrder, bombOrder);

            if(!surpressOutput){ 
                output << "[IssueOrder] Player: " << p -> getPlayerName() 
                    << " has issued a BOMB Order: " << bombOrder -> getEffect() << "\n";
            }
        }
    }

    void GameEngine::handleBlockadeOrder(Player* p, Player* neutralPlayer, Order* issuedOrder, bool surpressOutput, ostringstream& output) {

        //Player blockades their most outmatched owned territory
        vector<Territory*> borderTerritories = p -> getTerritoriesAdjacentToEnemy();
        
        if(borderTerritories.empty()){ 

            issuedOrder -> setEffect("[IssueOrder] Player: " + p -> getPlayerName() 
                                    + " can't issue a BLOCKADE order, as they own no territories bordering enemies.\n");
            if(!surpressOutput){ 

                output << "[IssueOrder] Player: " << p -> getPlayerName() 
                    << " can't issue a BLOCKADE order, as they own no territories bordering enemies.\n";
            
            }

            return; 
        
        }

        p -> addNeutralEnemy(neutralPlayer);
        sort(borderTerritories.begin(), borderTerritories.end(), Territory::territoryThreatCompareDescend);
        p -> removeNeutralEnemy(neutralPlayer);

        Territory* bestTarget = borderTerritories.front();
        if(bestTarget == nullptr){

            issuedOrder -> setEffect("[IssueOrder] Player: " + p -> getPlayerName() 
                                    + " can't issue a BLOCKADE order, the candidate territory for blockading is invalid.\n");
            if(!surpressOutput){

                output << "[IssueOrder] Player: " << p -> getPlayerName() 
                    << " can't issue a BLOCKADE order, the candidate territory for blockading is invalid.\n";

            }

            return;

        }

        Blockade* blockadeOrder = new Blockade(p, bestTarget, neutralPlayer);
        if(blockadeOrder != nullptr){
            OrderList* playerOrders = p -> getPlayerOrders();
            if(playerOrders == nullptr){ return; }

            playerOrders -> replaceOrder(issuedOrder, blockadeOrder);
            if(!surpressOutput){ 
                output << "[IssueOrder] Player: " << p -> getPlayerName() 
                    << " has issued a BLOCKADE Order: " << blockadeOrder -> getEffect() << "\n";
            }
        }
    }

    void GameEngine::handleAirliftOrder(Player* p, Player* neutralPlayer, Order* issuedOrder, bool surpressOutput, ostringstream& output) {

        //Player airlifts armies from the deepest region to the most threatened border territory

        vector<Territory*> owned = p -> getOwnedTerritories().getTerritories(); //Get Owned territories
        vector<Territory*> borderTerritories = p -> getTerritoriesAdjacentToEnemy(); //Get border territories

        if(owned.size() < 2 || borderTerritories.empty()) {  //This should not happen
            
            issuedOrder -> setEffect("[IssueOrder] Player: " + p -> getPlayerName() 
                                    + " can't issue an AIRLIFT order, as they don't have enough territories.\n");

            if(!surpressOutput){ 

                output << "[IssueOrder] Player: " << p -> getPlayerName() 
                    << " can't issue an AIRLIFT order, as they don't have enough territories.\n";

            }

            return; 

        }

        OrderList* playerOrders = p -> getPlayerOrders();
        if(playerOrders == nullptr || playerOrders -> getOrders().empty()) {  //This should not happen
        
            issuedOrder -> setEffect("[IssueOrder] Player: " + p -> getPlayerName() 
                                    + " can't issue an AIRLIFT order, as they have no available orders.\n");

            if(!surpressOutput){ 

                output << "[IssueOrder] Player: " << p -> getPlayerName() 
                    << " can't issue an AIRLIFT order, as they have no available orders.\n";

            }

            return; 
        }

        // ----------------------- Simplified Strategy ----------------------- //
        // Source: territory with the MOST armies (regardless of border/internal)
        // Target: MOST threatened border territory
        // Amount: Half of source armies (at least 1 moved, at least 1 left behind)
        // ------------------------------------------------------------------- //

        //Sort all owned territories by army count (descending) to pick the strongest as source
        sort(owned.begin(), owned.end(), Territory::territoryNumArmiesCompareDescend);

        Territory* source = owned.front();
        if(source == nullptr){ 

            issuedOrder -> setEffect("[IssueOrder] Player: " + p -> getPlayerName() 
                                     + " can't issue an AIRLIFT order, invalid source.\n");

            if(!surpressOutput){

                output << "[IssueOrder] Player: " << p -> getPlayerName()
                       << " can't issue an AIRLIFT order, invalid source.\n";

            }

            return;

        }

        //Compute how many armies to move: half, but ensure >=1 moved and >=1 left behind
        int srcArmies = source -> getNumArmies();
        if(srcArmies <= 1){

            issuedOrder -> setEffect("[IssueOrder] Player: " + p -> getPlayerName() 
                                     + " can't issue an AIRLIFT order, source has insufficient armies.\n");

            if(!surpressOutput){

                output << "[IssueOrder] Player: " << p -> getPlayerName()
                       << " can't issue an AIRLIFT order, source has insufficient armies.\n";

            }

            return;

        }

        int armiesToMove = max(1, srcArmies / 2);
        if(armiesToMove >= srcArmies){ armiesToMove = srcArmies - 1; } //Leave at least 1 behind

        //Temporarily add Neutral player to truce list for correct comparator behavior
        p -> addNeutralEnemy(neutralPlayer);

        //Sort border territories by threat level (descending) to pick the most threatened as target
        sort(borderTerritories.begin(), borderTerritories.end(), Territory::territoryThreatCompareDescend);

        //Remove the neutral player
        p -> removeNeutralEnemy(neutralPlayer);

        Territory* target = borderTerritories.front();

        if(target == nullptr){

            issuedOrder -> setEffect("[IssueOrder] Player: " + p -> getPlayerName() 
                                     + " can't issue an AIRLIFT order, invalid target.\n");

            if(!surpressOutput){

                output << "[IssueOrder] Player: " << p -> getPlayerName()
                       << " can't issue an AIRLIFT order, invalid target.\n";

            }

            return;

        }

        //Step 4: Create and issue the final Airlift Order
        Airlift* airliftOrder = new Airlift(p, source, target, armiesToMove);

        if(airliftOrder != nullptr) {

            playerOrders -> replaceOrder(issuedOrder, airliftOrder);

            if(!surpressOutput) { 

                output << "[IssueOrder] Player: " << p -> getPlayerName() 
                    << " has issued an AIRLIFT Order: " << airliftOrder -> getEffect();

            }

        }

}

    void GameEngine::handleNegotiateOrder(Player* p, Player* neutralPlayer, vector<Player*> neutrals, Order* issuedOrder, bool surpressOutput, ostringstream& output) {

        unordered_map<Player*, long long> enemyThreatLevel;
        vector<Territory*> borderTerrs = p -> getTerritoriesAdjacentToEnemy();
        if(borderTerrs.empty()) { return; }

        p -> addNeutralEnemy(neutralPlayer);
        sort(borderTerrs.begin(), borderTerrs.end(), Territory::territoryThreatCompareDescend);
        p -> removeNeutralEnemy(neutralPlayer);

        for(size_t i = 0; i < borderTerrs.size(); i++) {
            Territory* border = borderTerrs[i];
            if(border == nullptr){ continue; }

            int rankWeight = static_cast<int>(max(10.0, 100.0 - 7.5 * static_cast<double>(i)));

            for(Territory* neigh : border -> getNeighbors()) {

                if(neigh == nullptr){ continue; }

                Player* enemy = neigh -> getOwner();
                if(enemy == nullptr || enemy == p){ continue; }
                if(find(neutrals.begin(), neutrals.end(), enemy) != neutrals.end()){ continue; }

                enemyThreatLevel[enemy] += neigh -> getNumArmies() + rankWeight;
                Continent* playerCont = border -> getContinent();
                Continent* enemyCont = neigh -> getContinent();
                if(playerCont != nullptr && enemyCont == playerCont) { enemyThreatLevel[enemy] += 50; }

            }
        }

        unordered_map<Continent*, long long> masterHashes = gameMap -> getContinentLookupTable();
        unordered_map<Continent*, long long> playerContinentHashes = p -> getContinentLookupTablePlayer();
        const double BASE_CONTINENT_THREAT = 1500.0;

        for(pair<Continent*, long long> hashTableRow : masterHashes) {

            Continent* cont = hashTableRow.first;
            long long masterSum = hashTableRow.second;
            if(cont == nullptr){ continue; }

            long long playerSum = playerContinentHashes[cont];
            if(playerSum == 0){ continue; }

            int bonusValue = cont -> getBonusValue();
            if(bonusValue <= 0){ bonusValue = 1; }

            for(Player* enemy : players) {

                if(enemy == nullptr || enemy == p){ continue; }
                if(find(neutrals.begin(), neutrals.end(), enemy) != neutrals.end()){ continue; }

                const unordered_map<Continent*, long long>& enemyHashMap = enemy -> getContinentLookupTablePlayer();
                auto it = enemyHashMap.find(cont);
                if(it == enemyHashMap.end()){ continue; }

                long long enemySum = it -> second;
                if(enemySum <= 0){ continue; }

                double controlRatio = static_cast<double>(enemySum) / static_cast<double>(masterSum);
                long long continentThreat = static_cast<long long>(BASE_CONTINENT_THREAT * controlRatio * static_cast<double>(bonusValue));
                enemyThreatLevel[enemy] += continentThreat;
            }
        }

        if(enemyThreatLevel.empty()){ return; }

        Player* targetPlayer = nullptr;
        long long maxThreat = -1;
        for(pair<Player*, long long> entry : enemyThreatLevel) {
            if(entry.second > maxThreat) { 
                maxThreat = entry.second;
                targetPlayer = entry.first;
            }
        }

        if(targetPlayer == nullptr){ return; }

        Negotiate* negotiateOrder = new Negotiate(p, targetPlayer);
        if(negotiateOrder != nullptr) {
            OrderList* playerOrders = p -> getPlayerOrders();
            if(playerOrders == nullptr){ return; }

            playerOrders -> replaceOrder(issuedOrder, negotiateOrder);
            if(!surpressOutput){ 
                output << "[IssueOrder] Player: " << p -> getPlayerName()
                    << " has issued a NEGOTIATE Order with player: "
                    << targetPlayer -> getPlayerName()
                    << " (Total Threat Score = " << maxThreat
                    << ", Continent Weighted).\n";
            }
        }
    }

    void GameEngine::issueOrdersPhase(bool surpressOutput, ostringstream& output) {

        Player* neutralPlayer = nullptr;

        //Get neutral player
        for(Player* p : players) {

            if(p != nullptr && p -> getPlayerName() == "Neutral"){ 
                
                neutralPlayer = p; 
                break;

            }

        }

        for(Player* p : players) {

            if(p != nullptr && p == neutralPlayer){ continue; } //Skip null or neutral players

            if(!surpressOutput){ 
                
                output << "[IssueOrder] Player: " << p -> getPlayerName() 
                       << " is beginning their IssueOrder phase:\n\n"; 
            
            }

            //Let player decide what orders to issue (Deploy + Card + Advance + Defend)
            string issueOrderResults = p -> issueOrder(surpressOutput, deck, neutralPlayer);

            //Retrieve all orders issued by this player
            OrderList* playerOrders = p -> getPlayerOrders();
            vector<Order*> orders = playerOrders -> getOrders();

            if(orders.empty()){ 
                
                if(!surpressOutput){ 
                    output << "[IssueOrder] Player: " << p -> getPlayerName() 
                           << " issued no orders this turn.\n"; 
                }
                continue; 
            
            }

            //Intialize vector that contains all neutral enemies
            vector<Player*> neutrals = p -> getNeutralEnemies();
            neutrals.push_back(neutralPlayer); //Add neutral player

            //Iterate through every order and handle appropriately
            for(Order* issuedOrder : orders) {

                if(issuedOrder == nullptr){ continue; }

                switch(issuedOrder -> getOrderType()) {

                    case(OrderType::Deploy): {

                        //Deploy handled automatically in reinforcement phase, just print result
                        if(!surpressOutput){ 
                            output << "[IssueOrder] " << p -> getPlayerName() 
                                   << " has a DEPLOY order prepared.\n"; 
                        }

                    } break;

                    case(OrderType::Advance): {

                        //Advance orders are handled separately (Attack/Defend logic)
                        if(!surpressOutput){ 
                            output << "[IssueOrder] " << p -> getPlayerName() 
                                   << " has an ADVANCE order queued.\n"; 
                        }

                    } break;

                    case(OrderType::Bomb): { 

                        handleBombOrder(p, neutralPlayer, issuedOrder, surpressOutput, output);
                    
                    } break;

                    case(OrderType::Blockade): { 

                        handleBlockadeOrder(p, neutralPlayer, issuedOrder, surpressOutput, output);
                    
                    } break;

                    case(OrderType::Airlift): { 

                        handleAirliftOrder(p, neutralPlayer, issuedOrder, surpressOutput, output);
                    
                    } break;

                    case(OrderType::Negotiate): { 

                        handleNegotiateOrder(p, neutralPlayer, neutrals, issuedOrder, surpressOutput, output);
                    
                    } break;

                    default: {

                        if(!surpressOutput){ 
                            output << "[IssueOrder] Error: Unknown order type encountered while issuing orders for player: "
                                   << p -> getPlayerName() << ".\n"; 
                        }

                    } break;

                }

            }

            if(!surpressOutput) { output << issueOrderResults; }

        }

    }


    bool GameEngine::startAgain() {

        if(state != EngineState::Win) { //Can only restart after game has been won

            cout << "[Warning] You can only restart the game when someone has won." << endl;
            return false;

        }

        string response;

        while(true) {

            cout << "\n[WIN] The game has ended. Would you like to start a new game? (yes, y / no, n): ";
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            getline(std::cin, response);

            // Trim whitespace and convert to lowercase
            response.erase(remove_if(response.begin(), response.end(), ::isspace), response.end());
            transform(response.begin(), response.end(), response.begin(), ::tolower);

            clearGame(); //Clear all game data, since no matter the response, it must be reset

            if(response == "yes" || response == "y") {

                state = EngineState::Start;
                cout << "\n[GameEngine] The game has been reset. You may now load a new map and begin again.\n";
                return true;

            } else if(response == "no" || response == "n") {
              
                state = EngineState::End;
                cout << "\n[GameEngine] Exiting the game. See you next time!\n";
                return false;
            
            } else { cout << "\n[GameEngine] Invalid input. Please type 'yes' or 'no'."; }

        }

    }

    pair<int, string> GameEngine::computeReinforcementFor(Player* p, bool surpressOutput) const {
        
        // ---------------- Safety Checks ---------------- //
        if(p == nullptr || gameMap == nullptr) { 
            
            return {0, "[Reinforcement] Error: Invalid player or map pointer."}; 
        
        }

        // ---------------- Base Calculation ---------------- //
        int ownedTerrs = static_cast<int>(p -> getOwnedTerritories().size());
        int baseArmies = max(3, static_cast<int>(ownedTerrs / 3));
        int bonusArmies = 0;
        int reinforcementPool = p -> getReinforcementPool();

        unordered_map<Continent*, long long> continentLookupTable = gameMap -> getContinentLookupTable();

        ostringstream desc;
        if(!surpressOutput) {

            desc << "[Reinforcement] Calculating reinforcement for player: " 
                 << p -> getPlayerName() << "\n"
                 << "[Reinforcement] Base armies from territories (" 
                 << ownedTerrs << " owned): " << baseArmies << "\n";

        }

        // ---------------- Continent Bonus Calculation ---------------- //
        for(Continent* c : gameMap -> getContinents()) {

            if(c == nullptr){ continue; } //Skip null pointers

            //If player controls entire continent, apply bonus
            if(p -> controlsContinent(continentLookupTable, c)) {

                bonusArmies += c -> getBonusValue();

                if(!surpressOutput){

                    desc << "[Reinforcement] " << p -> getPlayerName()
                        << " receives +" << c -> getBonusValue()
                        << " bonus armies for controlling the continent: " 
                        << c -> getID() << "\n";

                }
            }
        }

        // ---------------- Final Computation ---------------- //
        int totalArmies = baseArmies + bonusArmies + reinforcementPool;

        if(!surpressOutput){

            desc << "[Reinforcement] Final reinforcement pool contribution: " 
                << reinforcementPool << "\n"
                << "[Reinforcement] Total armies calculated for deployment: " 
                << totalArmies << "\n";

        }

        return { totalArmies, desc.str() };
    
    }

    Player* GameEngine::findPlayerByName(const string& name) const {
        
        for (Player* p : players) {
            
            if(p != nullptr && p -> getPlayerName() == name){ return p; }

        }

        return nullptr;
    
    }

    /*-------------------------------------------Game Commands---------------------------------------------------*/

    string GameEngine::engineLoadMap(const string& path, bool surpressOutput) {
        
        if(!isCurrentStateCorrect(EngineState::Start, "loadmap")){ 
            
            return "[LoadMap] Error: Current state is not loadmap!"; 
        
        }

        clearGame(); //Reset current game data before loading new map

        //Get map from MapLoader
        MapLoader loader;
        int loaderReturnCode = loader.importMapInfo(path);

        string mapName = "Unknown Map";

        if(loaderReturnCode != MAP_FILE_NOT_FOUND){ mapName = loader.getMapName(); }//Extracted from MapLoader after parsing path

        // --- Handle Import Errors ---
        if(loaderReturnCode != MAP_OK){ 
            
            switch(loaderReturnCode){

                case MAP_FILE_NOT_FOUND:

                    return surpressOutput

                        ? "[LoadMap] Import failed: File not found."
                        : "[LoadMap] Import failed: File not found at path '" + path + "' for map '" + mapName + "'.";

                case MAP_INVALID_SECTION:
                
                    return surpressOutput
                        ? "[LoadMap] Import failed: Invalid or missing section headers."
                        : "[LoadMap] Import failed: Invalid or missing section headers in map file '" + path + "' (" + mapName + ").";

                case MAP_PARSE_ERROR:

                    return surpressOutput
                        ? "[LoadMap] Import failed: Syntax or formatting error while parsing."
                        : "[LoadMap] Import failed: Syntax or formatting error while parsing map file '" + path + "' (" + mapName + ").";

                default:

                    return surpressOutput
                        ? "[LoadMap] Import failed: Unknown error while importing."
                        : "[LoadMap] Import failed: Unknown error while importing map file '" + path + "' (" + mapName + ").";

                
            }
        
        }

        pair<int, Map*> loadResults = loader.loadMap();
        int loaderCode = loadResults.first;
        gameMap = loadResults.second;

        // --- Handle Build/Validation Errors ---
        if(loaderCode != MAP_OK || gameMap == nullptr){

            //Return state back to the start
            state = EngineState::Start;
            switch(loaderCode){

                case INVALID_MAP_PTR:
                
                    return surpressOutput
                        ? "[LoadMap] Load failed: Map pointer returned null."
                        : "[LoadMap] Load failed: Map pointer returned null for map '" + mapName + "' (" + path + ").";
                
                case INVALID_AUTHOR:
                    
                    return surpressOutput
                        ? "[LoadMap] Load failed: Missing or invalid author field."
                        : "[LoadMap] Load failed: Missing or invalid author field in map '" + mapName + "' (" + path + ").";

                case INVALID_IMAGE:

                    return surpressOutput
                        ? "[LoadMap] Load failed: Invalid image reference."
                        : "[LoadMap] Load failed: Map image reference invalid or missing in '" + mapName + "' (" + path + ").";

                case INVALID_WRAP:
                
                    return surpressOutput
                        ? "[LoadMap] Load failed: Invalid wrapping metadata."
                        : "[LoadMap] Load failed: Invalid wrapping information in '" + mapName + "' (" + path + ").";

                case INVALID_SCROLL:

                    return surpressOutput

                        ? "[LoadMap] Load failed: Invalid scroll field."
                        : "[LoadMap] Load failed: Scroll behavior field invalid or missing in '" + mapName + "' (" + path + ").";

                case INVALID_WARN:

                    return surpressOutput

                        ? "[LoadMap] Load failed: Warning detected during parsing."
                        : "[LoadMap] Load failed: Warning detected during parsing of map '" + mapName + "' (" + path + ").";

                case INVALID_CONTINENT:

                    return surpressOutput

                        ? "[LoadMap] Load failed: Invalid or disconnected continent definition."
                        : "[LoadMap] Load failed: One or more continents are disconnected or improperly defined in '" + mapName + "' (" + path + ").";

                case INVALID_TERRITORY:

                    return surpressOutput

                        ? "[LoadMap] Load failed: Invalid territory definition."
                        : "[LoadMap] Load failed: One or more territories are missing, duplicated, or mislinked in '" + mapName + "' (" + path + ").";

                case INVALID_MAP_STRUCTURE:

                    return surpressOutput
                        ? "[LoadMap] Load failed: Invalid map structure."
                        : "[LoadMap] Load failed: Map graph is not connected or violates structure rules in '" + mapName + "' (" + path + ").";

                default:

                    return surpressOutput
                        ? "[LoadMap] Load failed: Unknown structural error."
                        : "[LoadMap] Load failed: Unknown structural error while loading map '" + mapName + "' (" + path + ").";

            }

        }

        // --- Successful Load ---
        state = EngineState::MapLoaded;
        return surpressOutput ? 
            "[LoadMap] Map loaded successfully, and is ready for validation." :
            "[LoadMap] Map '" + mapName + "' successfully loaded from path '" + path + "' and is ready for validation.";

    }

    string GameEngine::engineValidateMap(bool surpressOutput) {

        if(!isCurrentStateCorrect(EngineState::MapLoaded, "validatemap")){ return "[ValidateMap] Error: Current state is not validatemap!"; }
        
        //Temporaraily set the state back to the start. Do this in case the validation fails
        
        //Validate Map
        if(gameMap == nullptr){ 
            
            state = EngineState::Start;
            return "[ValidateMap] No map to validate"; 
        
        }
        
        if(!(gameMap -> validate())){ 
        
            state = EngineState::Start;
            return "[ValidateMap] Invalid Map"; 
        
        }
        
        state = EngineState::MapValidated;
        return surpressOutput ?  "" : "Map has been successfully validated";

    }

    string GameEngine::engineAddPlayer(const string& name, bool surpressOutput) {
        
        if(state != EngineState::MapValidated && state != EngineState::PlayersAdded) {
            return "[AddPlayer] Error: Not in a state to add players!";
        }

        if(name.empty()) { return "[AddPlayer] Error: Player name cannot be empty!"; }

        //Check if player with that name already exists
        if(findPlayerByName(name) != nullptr){ return "[AddPlayer] Error: Player with that name already exists!"; }

        //Check if too many players have been added to the game
        if(playerQueue.size() >= gameMap -> getTerritories().size()) {

            return "[AddPlayer] Error: Too many players have been added! " + name + " has NOT been added!";

        }

        addPlayerToQueue(name);
        state = EngineState::PlayersAdded;

        return surpressOutput ? "" : "[AddPlayer] Player: " + name +  " has been added";

    }

    string GameEngine::engineGameStart(bool suppressOutput) {

        if(!isCurrentStateCorrect(EngineState::PlayersAdded, "gamestart")){ return "[GameStart] Error: Current state is not gamestart!"; }
        if(playerQueue.size() < 2){ return "[GameStart] Error: At least least 2 players are required to play"; }
        if(gameMap == nullptr){ return "[GameStart] Error: No map object was provided"; }

        ostringstream output;

        //----- Shuffle Players -----
        string shuffleResult = shufflePlayers(suppressOutput); //Don't bother shuffling the neutral player
        if(!suppressOutput){ output << shuffleResult; }

        //Always set the game to have a neutral player to handle order logic
        Player* neutral = new Player("Neutral", gameMap -> buildEmptyContinentHashmap());
        players.push_back(neutral);
        if(!suppressOutput){ output << "[GameStart]  Added neutral player." << endl; }

        //----- Randomly Assign Territories -----
        string assignResult = assignTerritories(suppressOutput);
        if(!suppressOutput){ output << assignResult; }

        //----- Instantiate deck with an appropriate number of cards -----
        if(deck == nullptr){ deck = new Deck(static_cast<int>(players.size())); }
        else{ deck -> setNumOfPlayers(static_cast<int>(players.size())); } //Update deck to have correct number of players

        //Raise reinforcement pool, assign 2 cards 
        for(Player* p : players) { 

            if(p -> getPlayerName() == "Neutral"){ continue; } //Skip neutral player, they shouldn't be considered

            //Set the reinforcement pool for a player to be 50
            p -> setReinforcementPool(50);

            if(!suppressOutput){ output << p -> getPlayerName() << " had 50 armies added to their reinforcement pool\n"; }

            //Give each player 2 cards to start with
            if(deck != nullptr && p -> getHand() != nullptr) { //Make sure deck and hand exist

                Card* c1 = deck -> draw();
                Card* c2 = deck -> draw();
                if(c1 != nullptr){ p -> getHand() -> addCardToHand(c1); }
                if(c2 != nullptr){ p -> getHand() -> addCardToHand(c2); }

                if(!suppressOutput){ 
                    
                    output << p -> getPlayerName() 
                           << " had the following cards added to their hand: " 
                           << c1 -> getTypeString() << ", "
                           << c2 -> getTypeString() << endl;
                
                }

            }

        }

        state = EngineState::AssignReinforcement;

        if(!suppressOutput){ output << "\n[GameEngine] Game setup complete, moving to reinforcement phase.\n"; }

        return suppressOutput ?  "[GameEngine] Game setup complete, moving to reinforcement phase." : output.str();

    }

    string GameEngine::engineAssignReinforcement(bool surpressOutputs) {

        ostringstream output;

        //Validate current state and prerequisites
        if(!isCurrentStateCorrect(EngineState::AssignReinforcement, "assignreinforcement")) {
            
            return "[AssignReinforcement] Error: Current state is not assignreinforcement!";
        
        }

        if(!gameMap) { return "[AssignReinforcement] Error: No map object was provided."; }

        if(!surpressOutputs) { output << "\n[AssignReinforcement] === Assigning Reinforcements to Players ===\n\n"; }

        //Assign reinforcement pools to each active player
        for(Player* p : players) {

            if(p == nullptr || p->getPlayerName() == "Neutral") { continue; } //Skip neutral player

            pair<int, string> reinforcementInfo = computeReinforcementFor(p, surpressOutputs);

            if(!surpressOutputs) { output << reinforcementInfo.second; }
            int numArmiesToAdd = reinforcementInfo.first;

            if(numArmiesToAdd <= 0) {

                if(!surpressOutputs) { //This shouldnt happen, this is a just in case
                    
                    output << "[AssignReinforcement] " << p->getPlayerName() << " receives 0 armies (no territories controlled).\n";
                }

                continue;
            
            }

            //Add to player's reinforcement pool
            int previousPool = p -> getReinforcementPool();
            p -> setReinforcementPool(previousPool + numArmiesToAdd);

            if(!surpressOutputs) {

                output << "[AssignReinforcement] " << p -> getPlayerName() << " receives " << numArmiesToAdd 
                       << " new armies (Pool: " << p -> getReinforcementPool() << ").\n\n";

            }

        }

        if (!surpressOutputs) {
            output << "[AssignReinforcement] === All Reinforcements Assigned ===\n\n";
        }

        //Advance game engine state 
        state = EngineState::IssueOrders;

        return surpressOutputs 
            ? "[AssignReinforcement] Reinforcement phase executed successfully."
            : output.str();

    }

    string GameEngine::engineIssueOrder(bool surpressOutput) {

        if(!isCurrentStateCorrect(EngineState::IssueOrders, "issueorder")){ 
            
            return "[IssueOrder] Error: Current state is not issueorder!"; 
        
        }

        ostringstream output; //Collect all output text for logging or printing

        if(!surpressOutput){ output << "\n[IssueOrder] === Issuing Orders for Each Player ===\n\n"; }

        issueOrdersPhase(surpressOutput, output);

        if(!surpressOutput){ output << "[IssueOrder] === All Orders Successfully Issued ===\n"; }

        return surpressOutput ? "[IssueOrder] Orders issued successfully." : output.str();

    }

    string GameEngine::engineEndIssueOrder(bool surpressOutput) {

        //Validate current state before proceeding
        if(!isCurrentStateCorrect(EngineState::IssueOrders, "endissueorders")) { 
            
            return "[EndIssueOrders] Error: Current state is not IssueOrders."; 
        
        }

        //Transition to next phase
        state = EngineState::ExecuteOrders;

        ostringstream output;

        if(!surpressOutput) {

            output << "[EndIssueOrders] All players: ";

            for(size_t i = 0; i < players.size(); ++i) {

                if(players[i] != nullptr) {

                    output << players[i] -> getPlayerName();
                    if(i < players.size() - 1){ output << ", "; } //Manage commas
                
                }

            }

            output << " have finished issuing orders.\n";            

        }

        output << "[EndIssueOrders] Transitioning from IssueOrders phase to ExecuteOrders phase.";

        return output.str();

    }

    string GameEngine::engineExecuteOrder(bool surpressOutput) {

        //State Validation 
        if(!isCurrentStateCorrect(EngineState::ExecuteOrders, "executeorder")){ 

            return "[ExecuteOrder] Error: Current state is not executeorder!"; 

        }

        ostringstream output; //Collect all output text for logging or printing

        if(!surpressOutput){

            output << "\n[ExecuteOrder] === Executing Orders for Each Player ===\n";

        }

        //Setup
        vector<Player*> ordered; //Vector of current player queue
        queue<Player*> tempQueue = playerQueue; //Temporary deep copy of player queue

        Player* neutralPlayer = nullptr;

        for(Player* p : players){ //Get neutral player

            if(p != nullptr && p -> getPlayerName() == "Neutral"){

                neutralPlayer = p;
                break;

            }

        }

        while(!tempQueue.empty()){ //Copy player queue to a vector for easier iteration later on in the method
            
            Player* tempPlayer = tempQueue.front();

            if(tempPlayer == neutralPlayer){ //Ignore neutral player in issuing turns

                tempQueue.pop();
                continue;

            }

            ordered.push_back(tempQueue.front());
            tempQueue.pop();

        }

        // Vector to collect players that must be deleted later
        vector<Player*> toDelete;

        //---------------------------- Execute Orders ----------------------------//

        // ---------------------------- First Pass: Execute ALL Deploy Orders (Round-Robin) ----------------------------
        bool anyDeploysRemaining = true;

        // Make a working copy of the player queue for Deploy phase iteration
        queue<Player*> deployQueue = playerQueue; 
        queue<Player*> activeDeployers; // Temporary to rebuild each pass

        while(anyDeploysRemaining) {

            anyDeploysRemaining = false; //Assume no Deploy orders remain until found

            // Iterate through all players currently in deployQueue
            while(!deployQueue.empty()) {

                Player* p = deployQueue.front();
                deployQueue.pop(); //Remove from front after peeking

                if(p == nullptr){ continue; } //Skip null players
                if(p -> getPlayerName() == "Neutral"){ continue; } //Skip neutral player

                OrderList* orders = p -> getPlayerOrders();
                if(!orders || orders -> size() == 0){ continue; } //Skip players with no orders

                Order* o = orders -> peek(); //Peek at next order
                if(o == nullptr){ orders -> removeOrder(0); continue; }

                //Check if this is a Deploy order
                if(o -> getOrderType() == OrderType::Deploy) {

                    anyDeploysRemaining = true; //At least one Deploy order found

                    //Validate order before execution
                    pair<bool, string> validationResult = o -> validate();

                    if(!validationResult.first){ //Invalid Deploy
                        
                        if(!surpressOutput){

                            output << "[ExecuteOrder] " << p -> getPlayerName()
                                   << " executes an invalid Deploy:\nReason: "
                                   << validationResult.second << "\n";
                        }

                        orders -> removeOrder(0);

                        //Check if next order is still a Deploy
                        Order* nextTop = (orders ? orders -> peek() : nullptr);
                        if(nextTop != nullptr && nextTop -> getOrderType() == OrderType::Deploy){ activeDeployers.push(p); }

                        continue;

                    }

                    //Execute Deploy order
                    if(!surpressOutput){

                        output << "[ExecuteOrder] " << p -> getPlayerName()
                               << " Executes a Deploy order\n"
                               << "[ExecuteOrder] Order effect: " << o -> getEffect() << "\n\n";

                    }

                    o -> execute();

                    if(!surpressOutput){

                        output << "[ExecuteOrder] " << p -> getPlayerName()
                               << " successfully executed a Deploy order.\n"
                               << o -> getEffect() << "\n\n";

                    }

                    orders -> removeOrder(0); //Remove executed Deploy order

                    //Check if next order is still a Deploy
                    Order* nextTop = (orders ? orders -> peek() : nullptr);
                    if(nextTop != nullptr && nextTop -> getOrderType() == OrderType::Deploy){ activeDeployers.push(p); }

                } else { //Not a Deploy order — skip for Deploy phase
                    
                    continue;

                }

            }

            //Rebuild deployQueue with only players that still have Deploys
            deployQueue = activeDeployers;
            while(!activeDeployers.empty()){ activeDeployers.pop(); }

        }

        // ---------------------------- Second Pass: Execute ALL Other Orders (Round-Robin) ----------------------------
        bool anyOrdersRemaining = true;

        // Make a working copy of the player queue for the main (non-Deploy) execution
        queue<Player*> mainQueue = playerQueue;
        queue<Player*> activeExecutors; // Temporary to rebuild each pass

        while(anyOrdersRemaining) {

            anyOrdersRemaining = false; //Assume no orders remain until found

            while(!mainQueue.empty()) {

                Player* p = mainQueue.front();
                mainQueue.pop(); //Pop player from front

                if(p == nullptr || p -> getPlayerName() == "Neutral"){ continue; } //Skip null or neutral players

                OrderList* orders = p -> getPlayerOrders();
                if(!orders || orders -> size() == 0){ continue; } //Skip players with no orders

                Order* o = orders -> peek(); //Peek at next order
                if(o == nullptr){ orders -> removeOrder(0); continue; }

                //Skip Deploys in this phase
                if(o -> getOrderType() == OrderType::Deploy){ continue; }

                anyOrdersRemaining = true; //At least one non-Deploy order found

                //Validate order before execution
                pair<bool, string> validationResult = o -> validate();

                if(!validationResult.first){ //Invalid order
                        
                    if(!surpressOutput){
                        output << "[ExecuteOrder]" << p -> getPlayerName()
                               << " executes an invalid order: " << *o 
                               << ". Reason:\n" << validationResult.second << "\n";
                    }

                    orders -> removeOrder(0);

                    //Check if next order exists
                    Order* nextTop = (orders ? orders -> peek() : nullptr);
                    if(nextTop != nullptr){ activeExecutors.push(p); }

                    continue;
                }

                //Execute valid order
                if(!surpressOutput){
                    output << "[ExecuteOrder] " << p -> getPlayerName()
                           << " Executes: " << *o << "\n"
                           << "[ExecuteOrder] Order effect: " << o -> getEffect() << "\n\n";
                }

                o -> execute();

                if(!surpressOutput){
                    output << "[ExecuteOrder] " << p -> getPlayerName()
                           << " is successful in issuing the order.\n"
                           << o -> getEffect() << "\n\n";
                }

                orders -> removeOrder(0); //Remove executed order

                //Check if next order exists
                Order* nextTop = (orders ? orders -> peek() : nullptr);
                if(nextTop != nullptr){ activeExecutors.push(p); }

            }

            //Rebuild mainQueue with only players that still have remaining orders
            mainQueue = activeExecutors;
            while(!activeExecutors.empty()){ activeExecutors.pop(); }

        }

        // ----------------------------- Clearing Truces / Collecting Cards -----------------------------
        for(Player* p: players){

            p -> clearNeutralEnemies(); //Clear out neutral enemy list at the end of every turn
            
            //Draw card if possible
            if(p ->getGenerateCardThisTurn()){

                Deck* gameDeck = deck;
                p -> getHand() -> addCardToHand(gameDeck -> draw());

            }

        }

        //---------------------------- Eliminate Players ----------------------------
        for(auto it = players.begin(); it != players.end();) { //Use an iterator to allow safe removal

            Player* p = *it; //Dereference iterator to get player

            if(p == nullptr || p -> getPlayerName() == "Neutral") { //Make SURE that neutral cant be eliminated
                
                it++;
                continue;

            }

            int terrCount = static_cast<int>(p->getOwnedTerritories().size());
            if(terrCount == 0) {

                if(!surpressOutput){

                    output << "[" << p -> getPlayerName() << "] has been eliminated from the game.\n";

                }

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

            if(p == nullptr){ continue; }
            if(p -> getPlayerName() == "Neutral"){ continue; }

            int terrCount = static_cast<int>(p -> getOwnedTerritories().size()); 
            if (terrCount > 0) { 

                activePlayers++;
                potentialWinner = p;

            }

        }

        if(activePlayers == 1 && potentialWinner != nullptr) { 

            hasWon = true;
            currentPlayer = potentialWinner;
        
        }

        //Second Check: See if the player who won controls the entire map
        const unordered_map<Continent*, long long>& refTable = gameMap -> getContinentLookupTable(); 

        for(Player* p : ordered) {

            if(p == nullptr){ continue; }
            if(find(toDelete.begin(), toDelete.end(), p) != toDelete.end()){ continue; }

            if(p -> controlsMap(refTable)) {
                currentPlayer = p;
                controlsMap = true;
                break;
            }
            
        }

        //---------------------------- Deferred Cleanup ----------------------------
        for(Player* dead : toDelete) {

            if(dead != nullptr) {

                if(!surpressOutput){

                    output << "[ExecuteOrders] Deleting player object for " << dead -> getPlayerName() << "\n";
                
                }

                delete dead;
            
            }
        
        }

        toDelete.clear();
        setTurn(getTurn() + 1); //Increment turn number by 1 at the end of the round

        //---------------------------- Phase Transition ----------------------------
        if(getTurn() > maxTurns) { //Check if the maximum number of turns has been reached

            state = EngineState::Win; //Force game termination

            if(!surpressOutput){

                output << "\n=== GAME ENDED: MAXIMUM TURN LIMIT REACHED ===\n"
                       << "[ExecuteOrder] The maximum number of turns (" << maxTurns << ") has been reached.\n"
                       << "[ExecuteOrder] No further rounds will be played. Game over.\n";

            }

            return surpressOutput ? "[ExecuteOrder] Game ended: maximum turns reached." : output.str();

        } else if(hasWon && controlsMap) {

            state = EngineState::Win;

            if(!surpressOutput){

                output << "\n=== WINNER DETECTED ===\n"
                       <<  currentPlayer -> getPlayerName() << " has won! They control all continents on the map!\n"
                       << "[ExecuteOrder] Win condition triggered.";

            }

            return surpressOutput ? "[ExecuteOrder] Win condition triggered." : output.str();

        } 
        else if(hasWon) {

            state = EngineState::Win;

            if(!surpressOutput){

                output << "\n=== WINNER DETECTED ===\n"
                       <<  currentPlayer -> getPlayerName() << " has won! They have elimnated all non-neutral enemies!\n"
                       << "[ExecuteOrder] Win condition triggered.";
            
            }

            return surpressOutput ? "[ExecuteOrder] Win condition triggered." : output.str();

        } else {

            if(!surpressOutput){

                output << "[ExecuteOrder] All orders executed successfully. Proceeding to next turn’s Reinforcement phase.\n";
            
            }

            return surpressOutput ? "[ExecuteOrder] Orders executed successfully." : output.str();

        }


    }

    string GameEngine::engineEndExecuteOrder(bool surpressOutput) {

        if(!isCurrentStateCorrect(EngineState::ExecuteOrders, "endexecuteorder")){ return "[EndExecuteOrder] Error: Current state is not endexecuteorder!"; }
        state = EngineState::AssignReinforcement;

        return surpressOutput ? 
            "[EndExecuteOrder] End of Turn: " + to_string(turn) + "\n[EndExecuteOrder] Returning to [AssignReinforcement]"
            : "[EndExecuteOrder] Returning to [AssignReinforcement]";
    
    }

    string GameEngine::engineWin() {

        if (!isCurrentStateCorrect(EngineState::Win, "win")){ return "[WIN] Error: Current state is not win!"; }

        ostringstream output; //Collect all output text for logging or printing
        
        bool startNewGame = startAgain();

        if(startNewGame) { 

            state = EngineState::Start;
            output << "[GameEngine] Starting new game";

            return output.str();

        } else {

            state = EngineState::End;
            output << "[GameEngine] Terminating Game.";

            return output.str();

        }
    
    }

    string GameEngine::engineEnd() {

        state = EngineState::End;
        return "[End] The program has now terminated. Thank you for playing!";

    }

    /*-----------------------------------------Player Queue Management-------------------------------------------*/

    void GameEngine::addPlayerToQueue(const std::string& playerName) {

        Player* newPlayer = new Player(playerName, gameMap -> buildEmptyContinentHashmap());
        players.push_back(newPlayer);
        playerQueue.push(newPlayer);

    }

    void GameEngine::addPlayers(int count) {
        
        for(int i = 1; i <= count; i++) { addPlayerToQueue("Player " + std::to_string(i)); }

    }

    Player* GameEngine::getNextPlayer() {

        return playerQueue.empty() ? nullptr : playerQueue.front();

    }

    void GameEngine::nextTurn() {

        if (playerQueue.empty()) {

            currentPlayer = nullptr;
            return;

        }

        //Remove player from front
        Player* front = playerQueue.front();
        playerQueue.pop();

        //Add player to back, set next player to front
        playerQueue.push(front);
        currentPlayer = playerQueue.front();
    
    }

    bool GameEngine::hasPlayers() const { return !playerQueue.empty(); }

    /*-------------------------------------------Gameplay Execution---------------------------------------------------*/

    string GameEngine::processCommand(const string& raw, const string& arg, bool surpressOutputs) {
        
        string cmd = raw;
        for(char& c : cmd){ c = static_cast<char>(tolower(c)); }

        if(cmd == "loadmap"){ return engineLoadMap(arg, surpressOutputs); }
        if(cmd == "validatemap"){ return engineValidateMap(surpressOutputs); }
        if(cmd == "addplayer"){ return engineAddPlayer(arg, surpressOutputs); }
        if(cmd == "gamestart"){ return engineGameStart(surpressOutputs); }
        if(cmd == "assignreinforcement"){ return engineAssignReinforcement(surpressOutputs); }
        if(cmd == "issueorder"){ return engineIssueOrder(surpressOutputs); }
        if(cmd == "endissueorders"){ return engineEndIssueOrder(surpressOutputs); }
        if(cmd == "executeorder"){ return engineExecuteOrder(surpressOutputs); }
        if(cmd == "endexecuteorder"){ return engineEndExecuteOrder(surpressOutputs); }
        if(cmd == "win"){ return engineWin(); }
        if(cmd == "end"){ return engineEnd(); }

        cerr << "[Error] Unknown command: " << raw << "\n";
        return "invalid";

    }

    string GameEngine::startupPhase(bool surpressOutput) {

        const string directory = "../Map/test_maps"; //Static root directory
        vector<string> mapFiles;

        //Collect ALL .map files recursively
        try {

            for(const auto& entry : fs::recursive_directory_iterator(directory)) { //Iterate over all folders in the test_map folder

                if (entry.is_regular_file() && entry.path().extension() == ".map") { //Check if a file in a folder is a .map file AND is a plain file

                    mapFiles.push_back(entry.path().string());

                }

            }

        } catch (const fs::filesystem_error& e) {

            cerr << "Filesystem error while scanning '" << directory << "': " << e.what() << endl;
            return "Filesystem error while scanning '" + directory + "': " + e.what() + "\n";

        }

        if (mapFiles.empty()) {

            cerr << "No .map files found under: " << directory << endl;
            return "No .map files found under: " + directory + "\n";
            
        }

        //Map selection and validation loop
        while(true) {

            int numMaps = mapFiles.size();
            if(surpressOutput == false){ cout << "===========================\n"; }
            cout << "There are a total of " << numMaps << " different maps to choose from. Please select a number from 1 to "
                << numMaps << ": ";

            int mapChoice;

            while(true) {

                if(!(cin >> mapChoice)) { //If the inputted choice is not an integer

                    //Handle Ctrl+Z / EOF input
                    if(cin.eof()) {

                        cout << "\n[StartupPhase] End of input detected (Ctrl+Z). Exiting startup phase.\n";
                        return "\n[StartupPhase] End of input detected (Ctrl+Z). Exiting startup phase.\n";

                    }

                    cin.clear(); // clear failbit
                    cin.ignore(numeric_limits<streamsize>::max(), '\n'); // discard invalid input
                    cout << "Invalid input. Please enter a number between 1 and " << numMaps << ": ";
                    continue;

                }

                // Handle out-of-range values
                if(mapChoice < 1 || (mapChoice > numMaps)){

                    cout << "Invalid selection. Please enter a number between 1 and " << numMaps << ": ";
                    continue;
                    
                }

                mapChoice--; //Decrement to get array bound for selected map 
                break;
            
            }

            string targetFile = mapFiles[mapChoice];

            if(surpressOutput == false){ cout << "\n[StartupPhase] Loading map: " << targetFile << endl; }

            // ----- Load and validate map -----
            string loadResult = engineLoadMap(targetFile, surpressOutput);
            if(surpressOutput == false){ cout << "[StartupPhase] " << loadResult << endl; }

            //If map loading failed, remove and re-prompt
            if(loadResult.find("failed") != string::npos || loadResult.find("Error") != string::npos) {

                cout << "[StartupPhase] The selected map could not be loaded successfully." << endl
                     << "[StartupPhase] Removing it from the available map list..." << endl;

                mapFiles.erase(mapFiles.begin() + mapChoice);

                if(mapFiles.empty()) {

                    cerr << "[StartupPhase] No valid maps remain. Exiting startup phase." << endl;
                    return "[StartupPhase] No valid maps remain. Exiting startup phase.\n";
                
                }

                cout << "[StartupPhase] Please select another map." << endl;
                continue;

            }

            string validateResult = engineValidateMap(surpressOutput);
            if(surpressOutput == false){ cout << "[StartupPhase] " << validateResult << endl; }

            //If map is invalid or validation failed, remove it and reprompt user
            if(validateResult.find("Invalid") != string::npos || 
               validateResult.find("Error") != string::npos) {

                cout << "[StartupPhase] The selected map is invalid or could not be validated." << endl
                     << "[StartupPhase] Removing it from the available map list..." << endl;

                mapFiles.erase(mapFiles.begin() + mapChoice);

                if(mapFiles.empty()) {

                    cerr << "[StartupPhase] No valid maps remain. Exiting startup phase." << endl;
                    return "[StartupPhase] No valid maps remain. Exiting startup phase.\n";
                
                }

                cout << "[StartupPhase] Please select another map." << endl;
                continue;

            }

            if(surpressOutput == false){ cout << "[StartupPhase] Map successfully loaded and validated!" << endl; }
            break;

        }

        // ----- Add players -----
        cout << "\nHow many players will participate (minimum 2, maximum 6): ";
        int numPlayers = 0;
        while(true) {
            
            if(!(cin >> numPlayers) || numPlayers < 2 || numPlayers > 6) {

                //Handle Ctrl+Z / EOF input
                if(cin.eof()) {

                    cout << "\n[StartupPhase] End of input detected (Ctrl+Z). Exiting startup phase.\n";
                    return "\n[StartupPhase] End of input detected (Ctrl+Z). Exiting startup phase.\n";
                
                }
                
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "Invalid input. Enter between 2 and 6: ";
                continue;
            
            }

            break;

        }

        cin.ignore(numeric_limits<streamsize>::max(), '\n'); //Clear input stream

        vector<string> addedNames; //Keep track of added player names

        for(int i = 0; i < numPlayers; ++i) {

            string playerName;

            while(true) {

                cout << "Enter name for Player " << (i + 1) << ": ";

                if(!getline(cin, playerName)) {
                    
                    //Handle Ctrl+Z / EOF input
                    if(cin.eof()) {

                        cout << "\n[StartupPhase] End of input detected (Ctrl+Z). Exiting startup phase.\n";
                        return "\n[StartupPhase] End of input detected (Ctrl+Z). Exiting startup phase.\n";
                    
                    }

                    cin.clear();
                    continue;
                }

                //Trim whitespace
                playerName.erase(remove_if(playerName.begin(), playerName.end(), ::isspace), playerName.end());

                //Check if empty
                if(playerName.empty()) {

                    cout << "Player name cannot be empty. Please enter a valid name." << endl;
                    continue;
                
                }

                //Check for duplicates
                if(find(addedNames.begin(), addedNames.end(), playerName) != addedNames.end()) {
                    
                    cout << "That name is already taken. Please choose another name." << endl;
                    continue;
                
                }

                //Add to player list if valid
                string addPlayerStatus = engineAddPlayer(playerName, surpressOutput);

                if(surpressOutput == false){ cout << addPlayerStatus << endl; }

                addedNames.push_back(playerName); //Record added name
                break;

            }

        }

        // ----- Set maximum number of turns -----
        cout << "\nPlease enter the maximum number of turns to be played before the game automatically ends: ";
        int turns = 0;
        while(true) {

            if(!(cin >> turns) || turns <= 0) {

                //Handle Ctrl+Z / EOF input
                if(cin.eof()) {
                    cout << "\n[StartupPhase] End of input detected (Ctrl+Z). Exiting startup phase.\n";
                    return "\n[StartupPhase] End of input detected (Ctrl+Z). Exiting startup phase.\n";
                }

                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "Invalid input. Enter a positive integer greater than 0: ";
                continue;

            }

            break;

        }

        this -> maxTurns = turns; //Store user-specified max turn limit

        if(surpressOutput == false){

            cout << "[StartupPhase] Maximum turns set to: " << maxTurns << endl;
        
        }

        //Start the Game
        string gameEngineResults = engineGameStart(surpressOutput);
        if(surpressOutput == false){
            
            cout << "\n[StartupPhase] Starting the game setup...\n" << endl;
            cout << gameEngineResults << endl; 
            cout << "[StartupPhase] Setup complete. Game ready to begin!" << endl;

        }

        return gameEngineResults;

    }

    void GameEngine::gameplayPhase(bool surpressOutput, string startLog, string filePath) {

        //---------------------------- Safety Checks ----------------------------
        if(gameMap == nullptr || players.empty()) { return; }

        //---------------------------- Log File Setup ----------------------------

        //Get current time
        auto now = chrono::system_clock::now();
        time_t currentTime = chrono::system_clock::to_time_t(now);
        tm localTime{};

        //Handle OS shenanigans
        #ifdef _WIN32
            localtime_s(&localTime, &currentTime);
        #else
            localtime_r(&currentTime, &localTime);
        #endif

        //Build filename with labeled components
        ostringstream filenameBuilder;
        filenameBuilder << filePath;

        //--------- Ensure directory exists before creating the file -------------
        try{ filesystem::create_directories(filePath); } 
        catch(const std::filesystem::filesystem_error& e){

            cerr << "[Error] Failed to create directory: " << filePath << "\nReason: " << e.what() << endl;
            return;

        }

        string currentMS = to_string(TimeUtil::getSystemTimeMillis() % 1000);
        string currentNS = to_string(TimeUtil::getSystemTimeNano() % 1000000);


        filenameBuilder << "Y" << (localTime.tm_year + 1900)
                        << "_M" << setw(2) << setfill('0') << (localTime.tm_mon + 1)
                        << "_D" << setw(2) << setfill('0') << localTime.tm_mday
                        << "_H" << setw(2) << setfill('0') << localTime.tm_hour
                        << "_M" << setw(2) << setfill('0') << localTime.tm_min
                        << "_S" << setw(2) << setfill('0') << localTime.tm_sec
                        << "_MS" << currentMS << "_NS" << currentNS
                        << "_SIMULATION.txt";

        string logFileName = filenameBuilder.str();
        ofstream logFile(logFileName);
        if(!logFile.is_open()) { return; }

        //---------------------------- Initialization ----------------------------
        bool gameOver = false;
        logFile << startLog << endl;
        cout << ("===============================\n");
        cout << ("=== ENTERING GAMEPLAY PHASE ===\n");
        cout << ("===============================\n");
        logFile << "[GameplayPhase] Beginning Turn " + to_string(getTurn()) << endl;

        //---------------------------- Main Game Loop ----------------------------
        while(!gameOver) {

            //---------------- Reinforcement Phase ----------------//
            string reinforceResult = engineAssignReinforcement(surpressOutput);
            logFile << reinforceResult << endl;

            //---------------- Issue Orders Phase ----------------//
            string issueResult = engineIssueOrder(surpressOutput);
            logFile << issueResult << endl;

            //---------------- End Issue Orders Phase ----------------//
            string endIssueResult = engineEndIssueOrder(surpressOutput);
            logFile << endIssueResult << endl;

            //---------------- Execute Orders Phase ----------------//
            string executeResult = engineExecuteOrder(surpressOutput);
            logFile << executeResult << endl;

            //Check if game ended (state == Win)
            if(state == EngineState::Win) {

                logFile << "\n[GameplayPhase] Win condition reached. Ending simulation." << endl;
                gameOver = true;
                break;

            }

            //---------------- End Execute Orders Phase ----------------//
            string endExecuteResult = engineEndExecuteOrder(surpressOutput);
            logFile << endExecuteResult << endl;

            //---------------- Increment Turn ----------------//
            logFile << "\n[GameplayPhase] Proceeding to Turn " + to_string(getTurn()) << endl;
            logFile << "---------------------------------------------------\n\n" << endl;
            logFile << "[GameplayPhase] Beginning Turn " + to_string(getTurn()) + "\n\n" << endl;


        }

        //---------------------------- Game End Summary ----------------------------
        logFile << "\n[GameplayPhase] Game ended after " + to_string(getTurn()) + " turns." << endl;
        if(state == EngineState::Win) { logFile << "[GameplayPhase] Final state: Win condition reached." << endl; } 
        else { logFile << "[GameplayPhase] Final state: Unexpected termination." << endl; }

        logFile << "[GameplayPhase] Simulation log saved to: " + logFileName << endl;
        logFile.close();

    }

    bool GameEngine::endPhase() {

        //---------------------------- State Validation ----------------------------
        if(state != EngineState::Win && state != EngineState::End) {

            cout << "[EndPhase] Error: Game cannot be ended because it has not reached the Win or End state." << endl;
            return false;

        }

        //---------------------------- WIN STATE HANDLING ----------------------------
        if(state == EngineState::Win) {

            cout << "\n====================================\n";
            cout << "             GAME OVER              \n";
            cout << "====================================\n";

            //Prompt Restart, clean up game engine if game must end
            bool restart = startAgain();

            if(restart) {

                state = EngineState::Start;
                cout << "\n[EndPhase] Game reset successful. Returning to Start state.\n";
                return true; //Restart requested

            } else {

                state = EngineState::End;
                cout << "\n[EndPhase] Game terminated by user. Exiting program.\n";
                return false; //Game fully ended

            }

        }

        //---------------------------- END STATE HANDLING ----------------------------
        if(state == EngineState::End) {

            cout << "\n====================================\n";
            cout << "          GAME TERMINATED           \n";
            cout << "====================================\n";
            cout << "[EndPhase] The game has already reached its End state.\n";
            cout << "[EndPhase] No further actions can be performed.\n";
            return false;

        }

        return false; //Failsafe — should never reach here

    }

    void GameEngine::simulateGame() {

        bool restart = false; // Tracks whether to restart after a full run

        do {

            //---------------------------- Phase 0: User Preference ----------------------------//
            bool surpressOutput = false;
            string response;

            cout << "=============================================\n";
            cout << "        WARZONE GAME SIMULATION START        \n";
            cout << "=============================================\n\n";

            cout << "Would you like to suppress console output? (yes/y to suppress, no/n for full output): ";
            getline(cin, response);

            //Trim whitespace and normalize to lowercase
            response.erase(remove_if(response.begin(), response.end(), ::isspace), response.end());
            transform(response.begin(), response.end(), response.begin(), ::tolower);

            if(response == "yes" || response == "y") {
                
                surpressOutput = true;
                cout << "\n[Simulation] Console output will be suppressed. All results will be written to the log file.\n";
            
            } else {
                
                cout << "\n[Simulation] Console output will be displayed during the simulation.\n";
            
            }

            //---------------------------- Phase 1: Startup ----------------------------//
            if(!surpressOutput){ cout << "\n=== STARTUP PHASE ===\n"; }
            string intialLog = startupPhase(surpressOutput);

            //---------------------------- Phase 2: Gameplay ---------------------------//
            if(!surpressOutput){

                cout << "\n=== GAMEPLAY PHASE ===\n";
                cout << "[Simulation] Entering main gameplay loop...\n";
            
            }

            gameplayPhase(surpressOutput, intialLog, "../GameEngine/Games/");

            //---------------------------- Phase 3: End Phase --------------------------//
            if(!surpressOutput){
                cout << "\n=== END PHASE ===\n";
                cout << "[Simulation] Entering end phase...\n";
            }

            restart = endPhase(); // Now controls loop continuation

            if(!surpressOutput){

                if(restart){ cout << "\n[Simulation] Restarting game upon user request.\n"; } 
                else{ cout << "\n[Simulation] Game fully ended. Exiting simulation.\n"; }
            
            }

            // If the player requested a restart, reset the game engine before the next run
            if(restart) {

                cout << "\n[GameEngine] The game has been reset. You may now load a new map and begin again.\n";

            }

        } while(restart); // Repeat full simulation if restart was requested

        cout << "\n=============================================\n";
        cout << "          WARZONE SIMULATION ENDED           \n";
        cout << "=============================================\n";

    }

}