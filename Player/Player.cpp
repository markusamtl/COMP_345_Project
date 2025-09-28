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

            os << pc.territories[i]->getID();
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

                territoryIndex[t->getID()] = t; 
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
        t->setOwner(newOwner);

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

    }

    Player::Player(const string& name) {

        this -> playerName = name;
        this -> neutralEnemies = {};
        this -> ownedTerritories = PlayerTerrContainer();
        this -> playerHand = new Hand();
        this -> playerOrders = new OrderList();
        this -> generateCardThisTurn = false;

    }

    Player::Player(const string& name, Hand* hand, OrderList* orders) {

        this -> playerName = name;
        this -> neutralEnemies = {};
        this -> ownedTerritories = PlayerTerrContainer();
        this -> playerHand = hand;
        this -> playerOrders = orders;
        this -> generateCardThisTurn = false;

    }

    Player::Player(const string& name, vector<string> neutralEnemies, PlayerTerrContainer ownedTerritories, Hand* hand, OrderList* orders, bool generateCard) {

        this->playerName = name;
        this->neutralEnemies = neutralEnemies;
        this->ownedTerritories = ownedTerritories;
        this->playerHand = hand;
        this->playerOrders = orders;
        this->generateCardThisTurn = generateCard;

        //Assign ownership of each territory to this player
        for (Territory* terr : this->ownedTerritories.getTerritories()) {

            //Make sure each territory exists
            if(terr != nullptr) { terr -> setOwner(this); }

        }

    }

    Player::~Player() {

        // Release hand and orders
        delete this->playerHand; 
        delete this->playerOrders;

        // Reset ownership for territories
        for(Territory* terr : this -> ownedTerritories.getTerritories()) {

            //Check if the current owner of the territory is the refered player, and the territory isn't null
            if(terr != nullptr && terr -> getOwner() == this) { terr -> setOwner(nullptr); }

        }

    }

    Player::Player(const Player& other) {

        this -> playerName = other.playerName;
        this -> neutralEnemies = other.neutralEnemies;
        this -> ownedTerritories = other.ownedTerritories;
        this -> playerHand = (other.playerHand ? new Hand(*other.playerHand) : nullptr);
        this -> playerOrders = (other.playerOrders ? new OrderList(*other.playerOrders) : nullptr);
        this -> generateCardThisTurn = other.generateCardThisTurn;

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

            // --- Fix: update ownership of copied territories ---
            for (Territory* terr : this->ownedTerritories.getTerritories()) {
                
                if (terr != nullptr) { terr->setOwner(this); }// overwrite previous owner with current Player

            }

        }

        return *this;

    }

    ostream& operator<<(ostream& os, const Player& p) {

        os << "Player Info:" << endl << "Name: " << p.playerName << endl
        << "Number of Territories: " << p.ownedTerritories.size() << "(";

        if (p.ownedTerritories.size() > 0) {

            os << "[";

            const auto& terrs = p.ownedTerritories.getTerritories();

            for (size_t i = 0; i < terrs.size(); i++) {

                if (terrs[i] != nullptr) {

                    os << terrs[i]->getID(); //Add territory to stream
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

    //-- Class Methods --//

    vector<Territory*> Player::toAttack() {
        
        unordered_set<Territory*> uniqueTargets; 

        for(Territory* currTerr : ownedTerritories.getTerritories()) {

            for(Territory* neighTerr : currTerr->getNeighbors()) {

                Player* neighOwner = neighTerr -> getOwner();

                //Check if a neighbouring territory is owned by a neutral enemy
                bool neutralEnemyCheck = false;

                if(neighOwner != nullptr) {

                    neutralEnemyCheck = find(neutralEnemies.begin(), neutralEnemies.end(), neighOwner -> getPlayerName()) != neutralEnemies.end();
                
                }
                
                if(neighOwner == this || neutralEnemyCheck) { continue; } //Skip if owned by self or neutral enemy

                uniqueTargets.insert(neighTerr);

            }

        }

        return vector<Territory*>(uniqueTargets.begin(), uniqueTargets.end()); 

    }

    void Player::toAttackPrint() const{

        // Make a copy of owned territories
        vector<Territory*> owned = ownedTerritories.getTerritories();

        //Sort by territory ID. Use comperator function to compare each ID.
        sort(owned.begin(), owned.end(), *(Territory::territoryIDCompare));

        // Iterate sorted territories
        for(Territory* terr : owned) {

            cout << "From: " << terr->getID() << ", " << playerName << " can attack: ";

            // Copy neighbors, so they can be sorted as well 
            vector<Territory*> neighbors = terr -> getNeighbors();

            // Sort neighbors by ID
            std::sort(neighbors.begin(), neighbors.end(), *(Territory::territoryIDCompare));

            bool foundEnemy = false;

            for(Territory* neighbor : neighbors) {

                if (neighbor->getOwner() != this) {

                    cout << neighbor->getID() << " (" << neighbor->getNumArmies() << " armies), ";
                    foundEnemy = true;

                }
                
            }

            if (!foundEnemy) { cout << "no enemies"; }

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

        int randomOrderNumber = static_cast<int>(TimeUtil::getSystemTimeNano() % 6); //Get a random order number
        Order* tempOrder = nullptr; //Instantiate a order pointer

        switch(randomOrderNumber) {

            case 0: tempOrder = new WarzoneOrder::Deploy(this, nullptr, 0); break; //Deploy
            case 1: tempOrder = new WarzoneOrder::Advance(this, nullptr, nullptr, 0); break; //Advance
            case 2: tempOrder = new WarzoneOrder::Bomb(this, nullptr); break; //Bomb
            case 3: tempOrder = new WarzoneOrder::Blockade(this, nullptr, nullptr); break; //Blockade
            case 4: tempOrder = new WarzoneOrder::Airlift(this, nullptr, nullptr, 0); break; //Airlift
            case 5: tempOrder = new WarzoneOrder::Negotiate(this, nullptr); break; //Negotiate

        }

        if(tempOrder != nullptr) {
        
            cout << this -> getPlayerName() << " has had an order added to their OrderList. Order Info:\n" << *tempOrder << endl;
            this -> getPlayerOrders() -> addOrder(tempOrder);

        }

    }

    void Player::addOwnedTerritories(Territory* territory) { ownedTerritories.addTerritory(territory, this); }

    void Player::removeOwnedTerritories(Territory* territory) { ownedTerritories.removeTerritory(territory); }

    void Player::addNeutralEnemy(const string& enemyName) {

        if(find(neutralEnemies.begin(), neutralEnemies.end(), enemyName) == neutralEnemies.end()) {

            neutralEnemies.push_back(enemyName); 

        }
    
    }

    void Player::removeNeutralEnemy(const string& enemyName) {

        auto enemyNameIndex = find(neutralEnemies.begin(), neutralEnemies.end(), enemyName); //Create iterator, easiest way to compare

        if (enemyNameIndex != neutralEnemies.end()) {

            neutralEnemies.erase(enemyNameIndex);

        } 

    }
    
}