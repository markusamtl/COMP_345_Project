#include "Player.h"
#include "../Order/Order.h"

using namespace std;

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
        this -> continentLookupTablePlayer = {};

    }

    Player::Player(const string& name, const unordered_map<Continent*, long long>& emptyHashMap) {

        this -> playerName = name;
        this -> neutralEnemies = {};
        this -> ownedTerritories = PlayerTerrContainer();
        this -> playerHand = new Hand();
        this -> playerOrders = new OrderList();
        this -> generateCardThisTurn = false;
        this -> continentLookupTablePlayer = emptyHashMap;

    }

    Player::Player(const string& name, vector<string> neutralEnemies, PlayerTerrContainer ownedTerritories, Hand* hand, OrderList* orders, 
                   bool generateCard, const unordered_map<Continent*, long long>& emptyHashMap) {

        this->playerName = name;
        this->neutralEnemies = neutralEnemies;
        this->ownedTerritories = ownedTerritories;
        this->playerHand = hand;
        this->playerOrders = orders;
        this->generateCardThisTurn = generateCard;
        this -> continentLookupTablePlayer = emptyHashMap;


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

            // Copy basic fields
            this->playerName = other.playerName;
            this->neutralEnemies = other.neutralEnemies;
            this->ownedTerritories = other.ownedTerritories;
            this->playerHand = (other.playerHand ? new Hand(*other.playerHand) : nullptr);
            this->playerOrders = (other.playerOrders ? new OrderList(*other.playerOrders) : nullptr);
            this->generateCardThisTurn = other.generateCardThisTurn;
            this -> continentLookupTablePlayer = other.continentLookupTablePlayer;

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

        if (p.ownedTerritories.size() > 0) {

            os << "([";

            const auto& terrs = p.ownedTerritories.getTerritories();

            for (size_t i = 0; i < terrs.size(); i++) {

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
                
                os << p.neutralEnemies[i];
                if(i < p.neutralEnemies.size() - 1) os << ", "; //Manage commas
            
            }
        
        }

        return os;

    }


    //-- Accessors and Mutators --//

    const string& Player::getPlayerName() const { return playerName; }

    void Player::setPlayerName(const string& name) { playerName = name; }

    const vector<string>& Player::getNeutralEnemies() const { return neutralEnemies; }

    void Player::setNeutralEnemies(const vector<string>& enemies) { neutralEnemies = enemies; }

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

    const unordered_map<WarzoneMap::Continent*, long long>& Player::getContinentLookupTablePlayer() const { return continentLookupTablePlayer; }

    void Player::setContinentLookupTablePlayer(const unordered_map<WarzoneMap::Continent*, long long>& newTable) {
        
        continentLookupTablePlayer = newTable;
    
    }

    //-- Class Methods --//

    vector<Territory*> Player::toAttack() {
        
        unordered_set<Territory*> uniqueTargets; 

        for(Territory* currTerr : ownedTerritories.getTerritories()) {

            if(currTerr == nullptr){ continue; } //Skip null territories

            for(Territory* neighTerr : currTerr->getNeighbors()) {

                if(neighTerr == nullptr){ continue; } //Skip null neighbours

                Player* neighOwner = neighTerr -> getOwner();

                //Check if a neighbouring territory is owned by a neutral enemy
                bool neutralEnemyCheck = false;

                if(neighOwner != nullptr) {

                    neutralEnemyCheck = find(neutralEnemies.begin(), neutralEnemies.end(), neighOwner -> getPlayerName()) != neutralEnemies.end();

                }
                
                if(neighOwner == this || neutralEnemyCheck || neighOwner == nullptr) { continue; } //Skip if owned by self, neutral enemy, or no one

                uniqueTargets.insert(neighTerr);

            }

        }

        return vector<Territory*>(uniqueTargets.begin(), uniqueTargets.end()); 

    }

    void Player::toAttackPrint() const {

        vector<Territory*> owned = ownedTerritories.getTerritories();

        // Sort owned territories by ID
        sort(owned.begin(), owned.end(), *(Territory::territoryIDCompare));

        for(Territory* terr : owned) {

            if(terr == nullptr){ continue; }

            cout << "From: " << terr -> getID() << ", " << playerName << " can attack: ";

            vector<Territory*> neighbors = terr->getNeighbors();

            // Sort neighbors
            sort(neighbors.begin(), neighbors.end(), *(Territory::territoryIDCompare));

            bool foundEnemy = false;
            for (Territory* neighbor : neighbors) {

                if(neighbor == nullptr){ continue; }

                Player* neighOwner = neighbor->getOwner();

                // Check neutrality
                bool neutralEnemyCheck = false;
                if(neighOwner != nullptr) {
                    
                    neutralEnemyCheck = find(
                        neutralEnemies.begin(),
                        neutralEnemies.end(),
                        neighOwner->getPlayerName()
                    ) != neutralEnemies.end();

                }

                if(neighOwner == this || neutralEnemyCheck || neighOwner == nullptr) { continue; } //Skip if owned by self, neutral enemy, or no one

                cout << neighbor->getID() << " (" << neighbor->getNumArmies() << " armies), ";
                foundEnemy = true;

            }

            if (!foundEnemy) {

                cout << "no enemies";

            }

            cout << endl;

        }

    }


    vector<Territory*> Player::toDefend() {

        return ownedTerritories.getTerritories();

    }

    void Player::toDefendPrint() const {

        // Make a copy of owned territories
        vector<Territory*> owned = ownedTerritories.getTerritories();

        // Sort by territory ID. Use comparator function to compare each ID.
        std::sort(owned.begin(), owned.end(), *(Territory::territoryIDCompare));

        // Print player info
        cout << "Player " << playerName << " can defend: ";

        if (owned.empty()) {

            cout << "no territories" << endl;

        } else {

            // Iterate sorted territories
            for (size_t i = 0; i < owned.size(); i++) {

                cout << owned[i]->getID() 
                    << " (" << owned[i]->getNumArmies() << " armies)";

                if (i < owned.size() - 1) { cout << ", "; }

            }

            cout << endl;

        }

    }


    void Player::issueOrder() {
        
        if(this -> playerOrders == nullptr){ return; }//Make sure the OrderList for the player exists

        //Track which order types are available to be issued by card. Advance is ALWAYS available.
        vector<pair<bool, int>> orderTypesAvailableByCard = {{true, 0}, {false, 1}, {false, 2}, {false, 3}, {false, 4}};

        if(playerHand != nullptr) {

            // Check for each card type in the player's hand
            orderTypesAvailableByCard[1].first = playerHand -> hasCardOfType(WarzoneCard::CardType::Bomb);
            orderTypesAvailableByCard[2].first = playerHand -> hasCardOfType(WarzoneCard::CardType::Blockade);
            orderTypesAvailableByCard[3].first = playerHand -> hasCardOfType(WarzoneCard::CardType::Airlift);
            orderTypesAvailableByCard[4].first = playerHand -> hasCardOfType(WarzoneCard::CardType::Diplomacy);

        }

        int randomOrder = -1; //Initialize to invalid order

        // Keep looping until we select a valid order
        while (randomOrder == -1 && !orderTypesAvailableByCard.empty()) {

            int randomOrderIndex = static_cast<int>(TimeUtil::getSystemTimeNano() % orderTypesAvailableByCard.size());
            auto orderTypeInfo = orderTypesAvailableByCard[randomOrderIndex];

            if(orderTypeInfo.first) {
            
                // Valid order type
                randomOrder = orderTypeInfo.second;
            
            } 
            else if (orderTypeInfo.second != 0) {

                // Remove invalid entries EXCEPT Advance (index 0 must stay)
                orderTypesAvailableByCard.erase(orderTypesAvailableByCard.begin() + randomOrderIndex);
            
            }

        }

        if (randomOrder == -1) {
            // Safety fallback (should never happen since Advance is always valid)
            randomOrder = 0;
        }

        Order* tempOrder = nullptr; //Instantiate a order pointer

        switch(randomOrder) {

            case 0: tempOrder = new WarzoneOrder::Advance(this, nullptr, nullptr, 0); break; //Advance
            case 1: tempOrder = new WarzoneOrder::Bomb(this, nullptr); break; //Bomb
            case 2: tempOrder = new WarzoneOrder::Blockade(this, nullptr, nullptr); break; //Blockade
            case 3: tempOrder = new WarzoneOrder::Airlift(this, nullptr, nullptr, 0); break; //Airlift
            case 4: tempOrder = new WarzoneOrder::Negotiate(this, nullptr); break; //Negotiate

        }

        if(tempOrder != nullptr) {
        
            this -> getPlayerOrders() -> addOrder(tempOrder);

        }

    }

    void Player::addOwnedTerritories(WarzoneMap::Territory* territory) {

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

    void Player::addNeutralEnemy(const string& enemyName) {

        if(find(neutralEnemies.begin(), neutralEnemies.end(), enemyName) == neutralEnemies.end()) {

            neutralEnemies.push_back(enemyName); 

        }
    
    }

    void Player::removeNeutralEnemy(const string& enemyName) {

        auto enemyNameIndex = find(neutralEnemies.begin(), neutralEnemies.end(), enemyName); //Create iterator, easiest way to compare

        if(enemyNameIndex != neutralEnemies.end()) {

            neutralEnemies.erase(enemyNameIndex);

        } 

    }

    vector<Territory*> Player::getBombCandidates() const {
        
        //Collect candidate target territories (owned by another player)
        unordered_map<int, Territory*> candidateMap; //Map to ensure unique territories
        vector<Territory*> ownedTerrs = this -> getOwnedTerritories().getTerritories(); //Get all of the player's territories

        //Get all neighboring territories owned by other players
        for(size_t i = 0; i < ownedTerrs.size(); i++) {
                            
            if(ownedTerrs[i] == nullptr){ continue; } //Skip null territories
            vector<Territory*> neighbors = ownedTerrs[i] -> getNeighbors(); //Get neighbors of that territory

            string playerName = this -> getPlayerName();

            for(Territory* t : neighbors) {
                                    
                //Skip null territories and territories owned by the player
                if(t == nullptr || t -> getOwner() == nullptr || t -> getOwner() -> getPlayerName() == playerName){ continue; }
                candidateMap[t -> getNumericTerrID()] = t; //Use numeric ID as key to ensure uniqueness

            }

        }

        // Convert unordered_map values to vector for return
        vector<Territory*> candidates;
        candidates.reserve(candidateMap.size());

        //Build return vector
        for(pair<int, Territory*> entry : candidateMap){ candidates.push_back(entry.second); }

        return candidates;

    }

    bool Player::controlsContinent(const unordered_map<Continent*, long long>& continentSums, Continent* cont) const {
        
        if(cont == nullptr){ return false; } //Check if continent exists

        //Check if both hashmaps contain the key (if either don't, don't bother continuing)
        if(continentLookupTablePlayer.count(cont) == 0 || continentSums.count(cont) == 0) { return false; }

        //Compare values directly
        return continentLookupTablePlayer.at(cont) == continentSums.at(cont);

    }

    bool Player::hasWon(const unordered_map<Continent*, long long>& continentSums) const {
        
        for (const pair<Continent* const, long long>& kv : continentSums) { //Check every continent

            Continent* cont = kv.first;

            if (!controlsContinent(continentSums, cont)) { return false; }

        }

        return true;

    }

}