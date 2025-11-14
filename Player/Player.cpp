#include "Player.h"
#include "../Order/Order.h"
#include "../PlayerStrategies/PlayerStrategies.h"

using namespace std;
using namespace WarzonePlayerStrategy;

namespace WarzonePlayer {

    // ================= PlayerTerrContainer ================= //

    //-- Constructors, Destructor, Copy Constructor, Assignment Operator, Stream Insertion Operator --//

    PlayerTerrContainer::PlayerTerrContainer() {

        this->territories = {};
        this->territoryIndex = {};
        
    }

    PlayerTerrContainer::PlayerTerrContainer(vector<Territory*> territories, unordered_map<string, Territory*> territoryIndex) {
        
        this -> territories = territories;
        this -> territoryIndex = territoryIndex;
    
    }

    PlayerTerrContainer::~PlayerTerrContainer() {

        this -> clear(); //Since territory deletion is handled by the map object

    }

    PlayerTerrContainer::PlayerTerrContainer(const PlayerTerrContainer& other) {
        
        territories = other.territories;
        territoryIndex = other.territoryIndex;
    
    }

    PlayerTerrContainer& PlayerTerrContainer::operator=(const PlayerTerrContainer& other) {

        if (this != &other) {

            territories = other.territories;
            territoryIndex = other.territoryIndex;

        }

        return *this;

    }

    ostream& operator<<(ostream& os, const PlayerTerrContainer& pc) {

        os << "Owned Territories (" << pc.territories.size() << "): [";

        for(size_t i = 0; i < pc.territories.size(); i++) {

            os << pc.territories[i] -> getID();
            if(i < pc.territories.size() - 1){ os << ", "; }

        }

        return os << "]\n";

    }

    //-- Accessor and Mutators --//

    const vector<Territory*>& PlayerTerrContainer::getTerritories() const {

        return territories;

    }

    void PlayerTerrContainer::setTerritories(const vector<Territory*>& newTerritories, Player* newOwner) {

        territories = newTerritories; //Overwrite old territory vector 
        territoryIndex.clear();       //Clear old hashmap

        for(Territory* t : territories) {
        
            if(t != nullptr){ 

                territoryIndex[t -> getID()] = t; 
                t->setOwner(newOwner);  //Update owner

            }
        
        }

    }

    const unordered_map<string, Territory*>& PlayerTerrContainer::getTerritoryIndex() const {

        return territoryIndex;
    
    }

    void PlayerTerrContainer::setTerritoryIndex(const unordered_map<string, Territory*>& newIndex, Player* newOwner) {
        
        territoryIndex = newIndex; //Overwrite old hashmap
        territories.clear();

        for (const pair<const string, Territory*>& currTerr : territoryIndex) {

            Territory* t = currTerr.second;
            if (t != nullptr) { //update owner
                
                territories.push_back(t);
                t -> setOwner(newOwner); 
            
            } 

        }

    }

    //-- Class Methods --//

    void PlayerTerrContainer::addTerritory(Territory* t, Player* newOwner) {

        if(this -> owns(t)){ return; } //Check if null or already exists
        
        territories.push_back(t);
        territoryIndex[t -> getID()] = t;

        //Update territory ownership
        t -> setOwner(newOwner);

    }

    void PlayerTerrContainer::removeTerritory(Territory* t) {
        
        if(t == nullptr){ return; } //Safety check

        //Lookup in the hashmap
        auto hashMapTerrIndex = territoryIndex.find( t -> getID()); //If it doesn't exist, get the end iterator
        if(hashMapTerrIndex == territoryIndex.end()) return;

        territoryIndex.erase(hashMapTerrIndex); //Remove territory from hashmap

        //Remove from the vector
        auto vectorTerrIndex = find(territories.begin(), territories.end(), t);

        if(vectorTerrIndex != territories.end()) { //Ensure that the territory to be removed exists in the vector

            territories.erase(vectorTerrIndex);
        
        }

        //Remove the player who currently owns the territory
        if(t -> getOwner() != nullptr) {

            t -> setOwner(nullptr);

        }

    }

    bool PlayerTerrContainer::owns(Territory* t) const {

        if(t == nullptr) { return false; }
        return territoryIndex.count(t->getID()) > 0;

    }

    void PlayerTerrContainer::clear() {

        territories.clear();
        territoryIndex.clear();

    }

    size_t PlayerTerrContainer::size() const {

        return territories.size();

    }

    // ================= Player ================= //

    //-- Constructors, Destructor, Copy Constructor, Assignment Operator, Stream Insertion Operator --//

    Player::Player() {

        this -> playerName = "";
        this -> neutralEnemies = {};
        this -> ownedTerritories = PlayerTerrContainer();
        this -> playerHand = new Hand();
        this -> playerOrders = new OrderList();
        this -> generateCardThisTurn = false;
        this -> reinforcementPool = 0;
        this -> continentLookupTablePlayer = {};
        this -> strategy = new HumanPlayerStrategy();

    }

    Player::Player(const string& name, const unordered_map<Continent*, long long>& emptyHashMap) {

        this -> playerName = name;
        this -> neutralEnemies = {};
        this -> ownedTerritories = PlayerTerrContainer();
        this -> playerHand = new Hand();
        this -> playerOrders = new OrderList();
        this -> generateCardThisTurn = false;
        this -> reinforcementPool = 0;
        this -> continentLookupTablePlayer = emptyHashMap;
        this -> strategy = new HumanPlayerStrategy();
    }

    Player::Player(const string& name, vector<Player*> neutralEnemies, PlayerTerrContainer ownedTerritories, Hand* hand, OrderList* orders, 
                   bool generateCard, int reinforcmentPool, const unordered_map<Continent*, long long>& emptyHashMap){


        this -> playerName = name;
        this -> neutralEnemies = neutralEnemies;
        this -> ownedTerritories = ownedTerritories;
        this -> playerHand = hand;
        this -> playerOrders = orders;
        this -> generateCardThisTurn = generateCard;
        this -> reinforcementPool = reinforcmentPool;
        this -> continentLookupTablePlayer = emptyHashMap;
        this -> strategy = new HumanPlayerStrategy();


        //Assign ownership of each territory to this player
        for (Territory* terr : this->ownedTerritories.getTerritories()) {

            //Make sure each territory exists
            if(terr != nullptr) { terr -> setOwner(this); }

        }

    }

    Player::Player(const string& name, vector<Player*> neutralEnemies, PlayerTerrContainer ownedTerritories, Hand* hand, OrderList* orders, 
                   bool generateCard, int reinforcmentPool, const unordered_map<Continent*, long long>& emptyHashMap, PlayerStrategy* strategy){


        this -> playerName = name;
        this -> neutralEnemies = neutralEnemies;
        this -> ownedTerritories = ownedTerritories;
        this -> playerHand = hand;
        this -> playerOrders = orders;
        this -> generateCardThisTurn = generateCard;
        this -> reinforcementPool = reinforcmentPool;
        this -> continentLookupTablePlayer = emptyHashMap;
        this -> strategy = strategy;


        //Assign ownership of each territory to this player
        for (Territory* terr : this->ownedTerritories.getTerritories()) {

            //Make sure each territory exists
            if(terr != nullptr) { terr -> setOwner(this); }

        }

    }

    Player::~Player() {

        // Release hand and orders
        delete this -> playerHand; 
        delete this -> playerOrders;
        delete this -> strategy;

        // Reset ownership for territories
        for(Territory* terr : this -> ownedTerritories.getTerritories()) {

            //Check if the current owner of the territory is the refered player, and the territory isn't null
            if(terr != nullptr && terr -> getOwner() == this) { terr -> setOwner(nullptr); }

        }

        continentLookupTablePlayer.clear();

    }

    Player::Player(const Player& other) {

        this -> playerName = other.playerName;
        this -> neutralEnemies = other.neutralEnemies;
        this -> ownedTerritories = other.ownedTerritories;
        this -> playerHand = (other.playerHand ? new Hand(*other.playerHand) : nullptr);
        this -> playerOrders = (other.playerOrders ? new OrderList(*other.playerOrders) : nullptr);
        this -> generateCardThisTurn = other.generateCardThisTurn;
        this -> continentLookupTablePlayer = other.continentLookupTablePlayer;
        this -> strategy = (other.strategy ? other.strategy->clone() : nullptr);

        //Update ownership of copied territories
        for (Territory* terr : this->ownedTerritories.getTerritories()) {

            if (terr != nullptr) { terr->setOwner(this); } //Overwrite old owner with the new Player

        }

    }

    Player& Player::operator=(const Player& other) {

        if (this != &other) {

            // Clean up existing dynamic allocations
            delete this -> playerHand;
            delete this -> playerOrders;
            delete this -> strategy;

            // Copy basic fields
            this->playerName = other.playerName;
            this->neutralEnemies = other.neutralEnemies;
            this->ownedTerritories = other.ownedTerritories;
            this->playerHand = (other.playerHand ? new Hand(*other.playerHand) : nullptr);
            this->playerOrders = (other.playerOrders ? new OrderList(*other.playerOrders) : nullptr);
            this->generateCardThisTurn = other.generateCardThisTurn;
            this -> continentLookupTablePlayer = other.continentLookupTablePlayer;
            this -> strategy = (other.strategy ? other.strategy->clone() : nullptr);

            // --- Fix: update ownership of copied territories ---
            for (Territory* terr : this->ownedTerritories.getTerritories()) {
                
                if (terr != nullptr) { terr->setOwner(this); }// overwrite previous owner with current Player

            }

        }

        return *this;

    }

    ostream& operator<<(ostream& os, const Player& p) {

        os << "Player Info:" << endl << "Name: " << p.playerName << endl
        << "Number of Territories: " << p.ownedTerritories.size();

        if(p.ownedTerritories.size() > 0) {

            os << "([";

            const auto& terrs = p.ownedTerritories.getTerritories();

            for(size_t i = 0; i < terrs.size(); i++) {

                if (terrs[i] != nullptr) {

                    os << terrs[i] -> getID(); //Add territory to stream
                    if (i < terrs.size() - 1) os << ", "; //Manage commas

                }

            }

            os << "])";

        }

        // Print hand directly if available
        os << endl << "Hand details: ";

        if(p.playerHand){ os << *p.playerHand; } else { os << "None"; }

        // Print order list directly if available
        os << endl << "Orders: ";

        if(p.playerOrders) { os << *p.playerOrders; } else { os << "None"; }
        
        os << endl << "Should a player be issued a card on this turn? " << (p.generateCardThisTurn ? "Yes!" : "No!");

        // Print neutral enemies
        os << endl << "At peace with: ";
        
        if(p.neutralEnemies.empty()) {
            
            os << "No players";
        
        } else {
            
            for(size_t i = 0; i < p.neutralEnemies.size(); i++) {
                
                os << p.neutralEnemies[i] -> getPlayerName();
                if(i < p.neutralEnemies.size() - 1) os << ", "; //Manage commas
            
            }
        
        }

        return os;

    }


    //-- Accessors and Mutators --//

    const string& Player::getPlayerName() const { return playerName; }

    void Player::setPlayerName(const string& name) { playerName = name; }

    const vector<Player*>& Player::getNeutralEnemies() const { return neutralEnemies; }

    void Player::setNeutralEnemies(const vector<Player*>& enemies) { neutralEnemies = enemies; }

    const PlayerTerrContainer& Player::getOwnedTerritories() const { return ownedTerritories; }

    void Player::setOwnedTerritories(const PlayerTerrContainer& newTerritories) {

        ownedTerritories = newTerritories;

        // Ensure all territories now belong to this player
        for (Territory* terr : ownedTerritories.getTerritories()) {

            if(terr != nullptr) { terr->setOwner(this); }

        }

    }

    Hand* Player::getHand() const { return playerHand; }

    void Player::setHand(Hand* newHand) {

        if(playerHand != nullptr){ delete playerHand; }
        playerHand = newHand;

    }

    OrderList* Player::getPlayerOrders() const { return playerOrders; }

    void Player::setPlayerOrders(OrderList* newOrders) {

        if(playerOrders != nullptr){ delete playerOrders; }
        playerOrders = newOrders;

    }

    bool Player::getGenerateCardThisTurn() const { return this -> generateCardThisTurn; }

    void Player::setGenerateCardThisTurn(bool flag) { this -> generateCardThisTurn = flag; }

    int Player::getReinforcementPool() const { return this -> reinforcementPool; }

    void Player::setReinforcementPool(int pool) { this -> reinforcementPool = pool; }

    const unordered_map<WarzoneMap::Continent*, long long>& Player::getContinentLookupTablePlayer() const { return continentLookupTablePlayer; }

    void Player::setContinentLookupTablePlayer(const unordered_map<WarzoneMap::Continent*, long long>& newTable) {
        
        continentLookupTablePlayer = newTable;
    
    }

    //----------------- Class Methods -------------------//

    unordered_map<Territory*, Territory*> Player::computeAttackMap() {
        unordered_map<Territory*, Territory*> attackMap; //Mapping of owned territory, best enemy target

        //Iterate through all owned territories
        for(Territory* currTerr : ownedTerritories.getTerritories()) {

            if(currTerr == nullptr) { continue; }

            vector<Territory*> enemyNeighbors;

            //Collect all valid enemy neighbors
            for(Territory* neighTerr : currTerr -> getNeighbors()) {

                if(neighTerr == nullptr) { continue; }

                Player* neighOwner = neighTerr -> getOwner();

                //Skip if owned by this player, neutral, or unowned
                bool isNeutral = false;

                if(neighOwner != nullptr) {

                    isNeutral = find(
                        neutralEnemies.begin(), 
                        neutralEnemies.end(), 
                        neighOwner
                    ) != neutralEnemies.end();

                }

                if(neighOwner == this || isNeutral || neighOwner == nullptr) { continue; }

                enemyNeighbors.push_back(neighTerr);

            }

            //Skip if this territory has no valid enemies nearby
            if(enemyNeighbors.empty()) {

                attackMap[currTerr] = nullptr;
                continue;

            }

            //Sort using the improved comparator (includes continent bonus logic)
            sort(enemyNeighbors.begin(), enemyNeighbors.end(), Territory::territoryAttackPriorityCompare);

            //Select the top-ranked attack target
            Territory* bestTarget = enemyNeighbors.front();
            attackMap[currTerr] = bestTarget;

        }

        return attackMap;        
    }

    unordered_map<Territory*, Territory*> Player::toAttack() {
        return strategy->toAttack(this);
    }

    string Player::toAttackString() {
        return strategy->toAttackString(this);
    }

    unordered_map<Territory*, Territory*> Player::computeDefendMap() {
        //Get all territories owned by this player that border at least one enemy territory
        vector<Territory*> borderTerritories = getTerritoriesAdjacentToEnemy();

        unordered_map<Territory*, Territory*> defenseMap; //Territory, ptr to next territory 1 layer closer to enemy
        unordered_set<Territory*> visited; //Visited territory
        queue<Territory*> q;

        //Initialize BFS with all border territories
        for(Territory* border : borderTerritories) {

            if(border == nullptr) { continue; } //Ignore null pointer

            visited.insert(border);
            q.push(border);
            defenseMap[border] = nullptr; //Border territories have no forward target, since they're on the border

        }

        //Perform BFS traversal inward through owned territories
        while(!q.empty()) {

            Territory* current = q.front(); //Get current territory
            q.pop();

            for(Territory* neighbor : current -> getNeighbors()) {

                if(neighbor == nullptr) { continue; } //Ignore null neighbours

                Player* owner = neighbor -> getOwner();

                //Only propagate through territories owned by this player
                if(owner == this && visited.find(neighbor) == visited.end()) {

                    visited.insert(neighbor);

                    //Assign neighbor’s "defense target" to point toward current
                    defenseMap[neighbor] = current;

                    //Continue exploring inward
                    q.push(neighbor);

                }

            }

        }

        return defenseMap;
    }

    unordered_map<Territory*, Territory*> Player::toDefend() {
        return strategy->toDefend(this);
    }

    string Player::toDefendString() {
        return strategy->toDefendString(this);
    }

    void Player::deployReinforcements(ostringstream& output, bool surpressOutput) {

        //If no reinforcements are available, skip this phase
        if(reinforcementPool <= 0) { return; }

        //Build defensive routing map (Territory -> next toward enemy)
        unordered_map<Territory*, Territory*> defenseMap = toDefend();

        //Collect frontline territories (those with nextTerritories that are nullptr)
        vector<Territory*> frontlines;
        for(pair<Territory*, Territory*> currTerr : defenseMap) {

            if(currTerr.second == nullptr) { frontlines.push_back(currTerr.first); }

        }

        if(frontlines.empty()) { //This should not happen

            output << "[IssueOrder] " << playerName
                << " has no frontline territories; skipping deployment of "
                << reinforcementPool << " armies.\n";

            reinforcementPool = 0;
            return;

        }

        //Sort by ascending army count so weakest frontlines get reinforced first
        sort(frontlines.begin(), frontlines.end(), Territory::territoryNumArmiesCompareAscend);

        int remainingArmies = reinforcementPool;
        output << "[IssueOrder] " << playerName
            << " is distributing " << reinforcementPool
            << " reinforcement armies.\n";

        //Deploy armies across frontlines ONLY
        for(size_t i = 0; i < frontlines.size() && remainingArmies > 0; i++) {

            Territory* target = frontlines[i];
            if(target == nullptr) { continue; }

            //Ensure fair minimum distribution (rounded up)
            int minArmies = static_cast<int>((remainingArmies + frontlines.size() - 1) / frontlines.size());

            //Randomly vary between minArmies and remainingArmies / 2 (never exceed remaining)
            int maxDeploy = max(minArmies, remainingArmies / 2);
            int armiesToDeploy = static_cast<int>(TimeUtil::getSystemTimeNano() % maxDeploy) + 1;
            armiesToDeploy = min(armiesToDeploy, remainingArmies);

            //Create and add Deploy order
            Order* deployOrder = new WarzoneOrder::Deploy(this, target, armiesToDeploy);
            this -> playerOrders -> addOrder(deployOrder);

            if(!surpressOutput) {

                output << "[IssueOrder] " << playerName
                    << " adds " << armiesToDeploy
                    << " army/armies to " << target -> getID()
                    << " (Remaining pool before deploy: " << remainingArmies << ")\n";

            }

            //Deduct from remaining pool
            remainingArmies -= armiesToDeploy;

        }

        reinforcementPool = 0;

        if(!surpressOutput) {

            output << "[IssueOrder] " << playerName
                << " has finished deploying all reinforcements.\n\n";

        }

    }

    void Player::issueAttackOrders(ostringstream& output, bool surpressOutput, Player* neutralPlayer) {

        //Get player’s border territories
        vector<Territory*> borderTerrs = getTerritoriesAdjacentToEnemy(); //Get border territories

        //Get per-territory preferred attack targets (Territory -> best enemy neighbor)
        unordered_map<Territory*, Territory*> attackMap = toAttack();

        //Determine if any NON-NEUTRAL attack is possible at all.
        bool nonNeutralAttackPossible = false; //Default set to false
        vector<Player*> neutrals = this -> getNeutralEnemies(); // players under truce / peaceful list
        neutrals.push_back(neutralPlayer); //Add neutral player from game engine

        for(Territory* source : borderTerrs) {

            if(source == nullptr) { continue; } //Skip null pointers

            int sourceArmies = source -> getNumArmies();

            //Only attack if current territory has more than 1 army (cannot move last defender)
            if(sourceArmies <= 1) { continue; }

            //Check all neighbors to see if a non-neutral valid target exists for this source
            for(Territory* neigh : source -> getNeighbors()) {

                if(neigh == nullptr) { continue; }

                Player* owner = neigh -> getOwner();
                if(owner == nullptr || owner == this) { continue; }

                // Skip if truce/neutral enemy
                if(find(neutrals.begin(), neutrals.end(), owner) != neutrals.end()) { continue; }

                int defenderArmies = neigh -> getNumArmies();

                //Only attack if enemy is not overwhelmingly strong (roughly within 70% strength range)
                if(defenderArmies <= static_cast<int>(sourceArmies * (6.0 / 7.0))) {
                    
                    nonNeutralAttackPossible = true;
                    break;

                }

            }

            if(nonNeutralAttackPossible) { break; }

        }

        //Iterate through all border territories and issue orders according to the above global rule
        for(Territory* source : borderTerrs) {

            if(source == nullptr) { continue; } //Skip null pointers

            int sourceArmies = source -> getNumArmies();

            //Only attack if current territory has more than 1 army (cannot move last defender)
            if(sourceArmies <= 1) { continue; }

            //Find this territory's preferred target from the attack map
            //Find this territory's preferred target from the attack map
            Territory* target = nullptr;

            if(attackMap.count(source) > 0) { target = attackMap[source]; } //Check if source exists in the unordered_map
            else { continue; } //No target computed for this source

            //If no valid target, skip
            if(target == nullptr) { continue; }

            //Validate target is still an enemy neighbor (defensive safety check)
            Player* targetOwner = target -> getOwner();
            if(targetOwner == nullptr || targetOwner == this) { continue; } //Make sure player doesn't own the owned territory 

            // If nonNeutral attacks are possible, avoid Neutral targets for this source.
            if(nonNeutralAttackPossible) {

                // If current preferred target is neutral or under truce, try to find an alternative non-neutral neighbor.
                if(find(neutrals.begin(), neutrals.end(), targetOwner) != neutrals.end()) {

                    Territory* altTarget = nullptr;

                    for(Territory* neigh : source -> getNeighbors()) {

                        if(neigh == nullptr) { continue; }

                        Player* owner = neigh -> getOwner();
                        if(owner == nullptr || owner == this) { continue; }

                        if(find(neutrals.begin(), neutrals.end(), owner) != neutrals.end()) { continue; } // still skip neutrals here

                        int defenderArmies = neigh -> getNumArmies();
                        if(defenderArmies > static_cast<int>(sourceArmies * (6.0 / 7.0))) { continue; } // too strong

                        // Prefer the neighbor with best attack priority (use existing comparator)
                        if(altTarget == nullptr || Territory::territoryAttackPriorityCompare(neigh, altTarget)) {
                            altTarget = neigh;
                        }

                    }

                    // If we found an alternate non-neutral target, use it; otherwise skip issuing from this source.
                    if(altTarget == nullptr) { continue; }
                    target = altTarget;
                    targetOwner = target -> getOwner();

                }

            } //If player behaviours are added in the futre, add else block here :)

            //Final defender check (recompute defender armies for chosen/adjusted target)
            int defenderArmies = target -> getNumArmies();

            //Only attack if enemy is not overwhelmingly strong (roughly within 70% strength range)
            if(defenderArmies > static_cast<int>(sourceArmies * (6.0 / 7.0))) { continue; } //Skip if defender too strong

            //Decide number of armies to send (always leave 1 army behind)
            int armiesToSend = max(1, sourceArmies - 1);

            //Create and add Advance order
            Order* advanceOrder = new WarzoneOrder::Advance(this, source, target, armiesToSend);
            this -> playerOrders -> addOrder(advanceOrder);

            if(!surpressOutput) {

                output << "[IssueOrder] " << playerName << " orders an Advance from "
                    << source -> getID() << " (" << sourceArmies << " armies)"
                    << " to attack " << target -> getID() << " ("
                    << target -> getNumArmies() << " defenders) with "
                    << armiesToSend << " armies.\n";

            }

        }

    }

    void Player::issueDefendOrders(ostringstream& output, bool surpressOutput) {

        //Obtain the defensive mapping (each territory points to the next closer to enemy)
        unordered_map<Territory*, Territory*> defenseMap = toDefend();

        if(defenseMap.empty()) {

            if(!surpressOutput) {
                output << "[IssueOrder] " << playerName
                    << " has no territories to defend.\n";
            }

            return;

        }

        //Iterate through all owned territories in the defense map
        for(const auto& entry : defenseMap) { //Compiler gets mad if I dont use auto here :( 

            Territory* source = entry.first; //Territory that will send reinforcements
            Territory* target = entry.second; //Territory closer to enemy (destination)

            if(source == nullptr || target == nullptr) { continue; } //Skip null entries, as well as frontlines

            int sourceArmies = source -> getNumArmies();
            if(sourceArmies <= 1) { continue; } //Cannot move last defender

            //Determine number of armies to send (keep at least one behind)
            int armiesToSend = max(1, sourceArmies - 1); //Send ALL armies

            //Create and add Advance order toward the border
            Order* defendAdvance = new WarzoneOrder::Advance(this, source, target, armiesToSend);
            this -> playerOrders -> addOrder(defendAdvance);

            if(!surpressOutput) {

                output << "[IssueOrder] " << playerName
                    << " reinforces from " << source -> getID()
                    << " (" << sourceArmies << " armies)"
                    << " toward " << target -> getID()
                    << " with " << armiesToSend << " armies.\n";

            }

        }

        if(!surpressOutput) {

            output << "[IssueOrder] " << playerName
                << " has completed all defensive reinforcements.\n\n";

        }

    }

    void Player::issueCardOrders(ostringstream& output, bool surpressOutput, Deck* gameDeck) {

        //Index mapping:
        //0: Bomb
        //1: Blockade
        //2: Airlift
        //3: Negotiate
        vector<pair<bool, int>> orderTypesAvailableByCard = { {false, 0}, {false, 1}, {false, 2}, {false, 3} };

        //Check if the player has corresponding cards for card-based orders
        if(playerHand != nullptr) {

            orderTypesAvailableByCard[0].first = playerHand -> hasCardOfType(WarzoneCard::CardType::Bomb);
            orderTypesAvailableByCard[1].first = playerHand -> hasCardOfType(WarzoneCard::CardType::Blockade);
            orderTypesAvailableByCard[2].first = playerHand -> hasCardOfType(WarzoneCard::CardType::Airlift);
            orderTypesAvailableByCard[3].first = playerHand -> hasCardOfType(WarzoneCard::CardType::Diplomacy);

        }

        //Filter out order types the player cannot logically issue
        for(int i = 0; i < 4; i++) {

            if(!canIssueCardOrder(i)) { orderTypesAvailableByCard[i].first = false; }

        }

        //Randomly pick a valid order type
        int randomOrder = -1;

        while(randomOrder == -1 && !orderTypesAvailableByCard.empty()) { 

            //Get random card
            int randomOrderIndex = static_cast<int>(TimeUtil::getSystemTimeNano() % orderTypesAvailableByCard.size());
            pair<bool, int> orderTypeInfo = orderTypesAvailableByCard[randomOrderIndex];

            if(orderTypeInfo.first == true) { randomOrder = orderTypeInfo.second; } //If the order is valid 
            else { orderTypesAvailableByCard.erase(orderTypesAvailableByCard.begin() + randomOrderIndex); }

        }

        if(randomOrder == -1) { //If there exist no valid card orders 

            if(!surpressOutput){ output << "[IssueOrder] " << playerName << " had no valid card orders to issue.\n"; }
            return;

        }

        //Create the selected order type
        Order* tempOrder = nullptr;

        //Get relevant card
        Card* usedCard = nullptr;

        switch(randomOrder) {

            case 0: 
                
                tempOrder = new WarzoneOrder::Bomb(this, nullptr); 
                usedCard = playerHand -> getCardOfType(WarzoneCard::CardType::Bomb);

            break;
            case 1: 
                
                tempOrder = new WarzoneOrder::Blockade(this, nullptr, nullptr);
                usedCard = playerHand -> getCardOfType(WarzoneCard::CardType::Blockade);

            break;
            case 2: 
            
                tempOrder = new WarzoneOrder::Airlift(this, nullptr, nullptr, 0); 
                usedCard = playerHand -> getCardOfType(WarzoneCard::CardType::Airlift);

            break;    
            case 3: 
            
                tempOrder = new WarzoneOrder::Negotiate(this, nullptr); 
                usedCard = playerHand -> getCardOfType(WarzoneCard::CardType::Diplomacy);

            break;
            default: break; //This should NEVER happen

        }

        //Remove used card from player's hand, return to game deck
        playerHand -> removeCardFromHand(usedCard);
        gameDeck -> returnToDeck(usedCard);

        //Inform that a card has been used
        if(!surpressOutput){
                
            output << "[IssueOrder] " << playerName 
                << " has used a " 
                << usedCard -> getTypeString()
                << " card.\n";

        }

        //Add the order to the player's list
        if(tempOrder != nullptr) {

            this -> playerOrders -> addOrder(tempOrder);

            if(!surpressOutput){
                
                output << "[IssueOrder] " << playerName 
                    << " issued a "
                    << tempOrder -> getOrderTypeString()
                    << " order.\n";

            }

        } else {

            if(!surpressOutput){ output << "[IssueOrder] " << playerName << " failed to issue an order from a card.\n"; }

        }

    }

    string Player::issueOrder(bool surpressOutput, Deck* gameDeck, Player* neutralPlayer) {
        ostringstream output; //Collect all log outputs here
        return strategy->issueOrder(this, output, surpressOutput, gameDeck, neutralPlayer);
    }

    bool Player::canIssueCardOrder(int orderType) const {

        // Safety: if the player owns nothing, no card makes sense
        const vector<Territory*>& owned = this -> getOwnedTerritories().getTerritories();

        switch(orderType) {

            case 0: { // Bomb: must have at least one valid enemy target (not under truce)
                
                vector<Territory*> bombTargets = getBombCandidates();
                return !bombTargets.empty();

            }

            case 1: { // Blockade: must own at least one territory (non-null) with >=1 army

                if(owned.empty()){ return false; }
                
                for(Territory* t : owned) {

                    if(t == nullptr) { continue; }

                    //Player can only blockade their own territory, they have to have more than 1 army to do it
                    if(t -> getOwner() == this && t -> getNumArmies() >= 1) { return true; }

                }

                return false;
            
            }

            case 2: { // Airlift: need a valid source (owned, >1 army so 1 can remain) and a distinct owned target
                
                if(owned.size() < 2) { return false; }

                // Find any source with enough armies
                for(Territory* src : owned) {

                    if(src == nullptr) { continue; }

                    // Keep at least 1 behind
                    if(src -> getOwner() == this && src -> getNumArmies() > 1) {

                        // Need a different owned target (any)
                        for(Territory* dst : owned) {

                            if(dst == nullptr) { continue; }
                            if(dst == src) { continue; }
                            if(dst -> getOwner() != this) { continue; }

                            return true; // found (src, dst)

                        }

                    }

                }

                return false;
            
            }

            case 3: { // Negotiate (Diplomacy): must have at least one *other* active opponent not already neutral
                // Heuristic: scan neighbors of our owned territories for an opponent we’re not at truce with.
                if(owned.empty()) { return false; }

                const vector<Player*>& neutrals = this -> getNeutralEnemies();

                for(Territory* mine : owned) {

                    if(mine == nullptr) { continue; }

                    for(Territory* n : mine -> getNeighbors()) {

                        if(n == nullptr) { continue; }

                        Player* other = n -> getOwner();
                        if(other == nullptr || other == this) { continue; }

                        // If not already neutral, we can propose a negotiate
                        if(find(neutrals.begin(), neutrals.end(), other) == neutrals.end()) { return true; }

                    }
                }

                // No visible opponents → negotiating does nothing
                return false;
            }

            default: {
                return false;
            }

        }

    }


    void Player::clearNeutralEnemies() {

        this -> neutralEnemies.clear();
        
    }

    void Player::addOwnedTerritories(Territory* territory) {

        //Prevent duplicate / null ownership
        if(territory == nullptr || territory -> getOwner() == this) { return; }

        //If another player owns it, remove it from their list first
        Player* prevOwner = territory -> getOwner();
        if (prevOwner != nullptr) {

            prevOwner -> removeOwnedTerritories(territory);
            
        }

        // Assign to player via ownedTerritories
        ownedTerritories.addTerritory(territory, this);

        WarzoneMap::Continent* cont = territory -> getContinent();
        if(cont != nullptr) { continentLookupTablePlayer[cont] += territory -> getNumericTerrID(); } //Increment continent score by territory ID

    }

    void Player::removeOwnedTerritories(WarzoneMap::Territory* territory) {
        
        //Prevent duplicate / null ownership
        if (territory == nullptr || territory -> getOwner() != this) { return; }

        // Remove from player
        ownedTerritories.removeTerritory(territory);

        // Update continent sums
        WarzoneMap::Continent* cont = territory -> getContinent();
        if(cont != nullptr) { continentLookupTablePlayer[cont] -= territory -> getNumericTerrID(); }  //Decrement continent score by territory ID

    }

    void Player::addNeutralEnemy(Player* enemyName) {

        if(find(neutralEnemies.begin(), neutralEnemies.end(), enemyName) == neutralEnemies.end()) {

            neutralEnemies.push_back(enemyName); 

        }
    
    }

    void Player::removeNeutralEnemy(Player* enemyName) {

        auto enemyNameIndex = find(neutralEnemies.begin(), neutralEnemies.end(), enemyName); //Create iterator, easiest way to compare

        if(enemyNameIndex != neutralEnemies.end()) {

            neutralEnemies.erase(enemyNameIndex);

        } 

    }

    vector<Territory*> Player::getTerritoriesAdjacentToEnemy() const {

        unordered_set<Territory*> adjacentToEnemySet;

        // Loop through each owned territory
        for(Territory* owned : ownedTerritories.getTerritories()) {

            //Should not happen, check just in case
            if(owned == nullptr){ continue; }

            // Check all neighboring territories
            for(Territory* neighbor : owned -> getNeighbors()) {

                //Should not happen, check just in case
                if(neighbor == nullptr) continue;

                Player* neighborOwner = neighbor -> getOwner();

                //If neighbor exists, and belongs to someone else (enemy), mark this territory
                if(neighborOwner != nullptr && neighborOwner != this) {

                    adjacentToEnemySet.insert(owned);
                    break; //No need to check other neighbors for this territory, it is adjacent to at least 1 enemy

                }

            }

        }

        //Convert to a vector safely
        vector<Territory*> territoriesAdjacentToEnemy;
        territoriesAdjacentToEnemy.reserve(adjacentToEnemySet.size());

        for(Territory* t : adjacentToEnemySet) {
            
            if(t != nullptr) { territoriesAdjacentToEnemy.push_back(t); }
        
        }

        //Sort by ascending army count (weakest borders first)
        sort(territoriesAdjacentToEnemy.begin(), territoriesAdjacentToEnemy.end(), Territory::territoryNumArmiesCompareAscend);

        return territoriesAdjacentToEnemy;
        
    }

    vector<Territory*> Player::getSourcesWithManyArmies() const {

        vector<Territory*> validSources;
        vector<Territory*> ownedTerrs = ownedTerritories.getTerritories();

        //Iterate through each owned territory
        for(Territory* t : ownedTerrs) {

            //Skip invalid or inactive territories
            if(t == nullptr) { continue; }

            Player* owner = t -> getOwner();

            //Safety: only include territories still owned by this player
            if(owner == nullptr || owner != this) { continue; }

            //A valid source must have more than one army to send (keep at least one behind)
            if(t -> getNumArmies() > 1) { validSources.push_back(t); }

        }

        //Sort descending by army count so strongest come first
        sort(validSources.begin(), validSources.end(), Territory::territoryNumArmiesCompareDescend);

        return validSources;
    }

    vector<Territory*> Player::getBombCandidates() const {

        unordered_set<Territory*> candidateSet; //Ensure uniqueness of bombing targets
        const vector<Player*>& neutrals = this -> getNeutralEnemies(); //Truce list

        //Get all territories owned by player that are adjacent to at least one enemy
        vector<Territory*> frontlines = this -> getTerritoriesAdjacentToEnemy();

        //Iterate through each frontline territory
        for(Territory* owned : frontlines) {

            //Skip null pointers
            if(owned == nullptr) { continue; }

            //Check all neighboring territories of this frontline territory
            for(Territory* neighbor : owned -> getNeighbors()) {

                if(neighbor == nullptr) { continue; }

                Player* neighborOwner = neighbor -> getOwner();

                //Skip invalid or friendly neighbors
                if(neighborOwner == nullptr || neighborOwner == this) { continue; }

                //Skip neighbors belonging to truce partners
                if(find(neutrals.begin(), neutrals.end(), neighborOwner) != neutrals.end()) { continue; }

                //Skip neighbors with <= 1 army (bombing would have no meaningful effect)
                if(neighbor -> getNumArmies() <= 1) { continue; }

                //Otherwise, this is a valid bombing target
                candidateSet.insert(neighbor);

            }

        }

        //Convert unordered_set to vector for return
        vector<Territory*> candidates;
        candidates.reserve(candidateSet.size());

        for(Territory* t : candidateSet) {
        
            if(t != nullptr) { candidates.push_back(t); }
        
        }

        //Prioritize largest army counts first (high-value bomb targets)
        sort(candidates.begin(), candidates.end(), Territory::territoryNumArmiesCompareDescend);

        return candidates;

    }

    bool Player::controlsContinent(const unordered_map<Continent*, long long>& continentSums, Continent* cont) const {
        
        if(cont == nullptr){ return false; } //Check if continent exists

        //Check if both hashmaps contain the key (if either don't, don't bother continuing)
        if(continentLookupTablePlayer.count(cont) == 0 || continentSums.count(cont) == 0) { return false; }

        //Compare values directly
        return continentLookupTablePlayer.at(cont) == continentSums.at(cont);

    }

    bool Player::controlsMap(const unordered_map<Continent*, long long>& continentSums) const {
        
        for (const pair<Continent* const, long long>& kv : continentSums) { //Check every continent

            Continent* cont = kv.first;

            if (!controlsContinent(continentSums, cont)) { return false; }

        }

        return true;

    }

    PlayerStrategyType Player::getStrategyType() const {
        return strategy->getStrategyType();
    }

    void Player::setStrategyType(PlayerStrategyType strategyType) {
        delete strategy; //Clean up existing strategy

        //Create new strategy based on the specified type
        switch (strategyType) {
            case PlayerStrategyType::HUMAN:
                strategy = new HumanPlayerStrategy();
                break;
            case PlayerStrategyType::AGGRESSIVE:
                strategy = new AggressivePlayerStrategy();
                break;
            case PlayerStrategyType::BENEVOLENT:
                strategy = new BenevolentPlayerStrategy();
                break;
            case PlayerStrategyType::NEUTRAL:
                strategy = new NeutralPlayerStrategy();
                break;
            case PlayerStrategyType::CHEATER:
                strategy = new CheaterPlayerStrategy();
                break;
            default:
                strategy = new HumanPlayerStrategy(); //Default to human if unknown
                break;
        }
    }

    string Player::getStrategyTypeString() const {
        switch (strategy->getStrategyType()){
            case PlayerStrategyType::HUMAN: return "Human";
            case PlayerStrategyType::AGGRESSIVE: return "Aggressive";
            case PlayerStrategyType::BENEVOLENT: return "Benevolent";
            case PlayerStrategyType::NEUTRAL: return "Neutral";
            case PlayerStrategyType::CHEATER: return "Cheater";
            default: return "Unknown";
        }
    }
}