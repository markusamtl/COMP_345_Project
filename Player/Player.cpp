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
        this -> playerHand = nullptr;
        this -> playerOrders = nullptr;
        this -> generateCardThisTurn = false;

    }

    Player::Player(const string& name) {

        this -> playerName = name;
        this -> neutralEnemies = {};
        this -> ownedTerritories = PlayerTerrContainer();
        this -> playerHand = nullptr;
        this -> playerOrders = nullptr;
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
        this->ownedTerritories = ownedTerritories;  // copy the container
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

        os << "Player(Name: " << p.playerName
        << ", Territories: " << p.ownedTerritories.size();

        if (p.ownedTerritories.size() > 0) {

            os << " [";

            const auto& terrs = p.ownedTerritories.getTerritories();

            for (size_t i = 0; i < terrs.size(); i++) {

                if (terrs[i] != nullptr) {

                    os << terrs[i]->getID(); //Add territory to stream
                    if (i < terrs.size() - 1) os << ", "; //Manage commas

                }

            }

            os << "]";

        }

        // Print hand directly if available
        os << ", Hand details: ";

        if(p.playerHand) { os << *p.playerHand; } else { os << "None"; }

        // Print order list directly if available
        os << ", Orders: ";

        if(p.playerOrders) { os << *p.playerOrders; } else { os << "None"; }
        
        os << ", Should a player be issued a card? " << (p.generateCardThisTurn ? "Yes!" : "No!")
        << ")";

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

    OrderList* Player::getOrders() const { return playerOrders; }

    void Player::setOrders(OrderList* newOrders) {

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

    void Player::toAttackPrint() {

        for(Territory* currTerr : ownedTerritories.getTerritories()) {

            cout << "From: " << currTerr->getID() << ", " << playerName << " can attack: ";

            bool foundEnemy = false;

            for(Territory* neighTerr : currTerr->getNeighbors()) {

                Player* neighOwner = neighTerr -> getOwner();

                //Check if a neighbouring territory is owned by a neutral enemy
                bool neutralEnemyCheck = false;

                if(neighOwner != nullptr) {

                    neutralEnemyCheck = find(neutralEnemies.begin(), neutralEnemies.end(), neighOwner -> getPlayerName()) != neutralEnemies.end();
                
                }

                if(neighOwner == this || neutralEnemyCheck) { continue; } //Skip if owned by self or neutral enemy

                cout << neighTerr->getID() << " (" << neighTerr->getNumArmies() << " armies), ";
                
                foundEnemy = true;

            }

            if (!foundEnemy) { cout << "no enemies"; }

            cout << "\n";

        }

    }

    vector<Territory*> Player::toDefend() {

        return ownedTerritories.getTerritories();

    }

    void Player::toDefendPrint() {

        vector<Territory*> ownedTerrs = ownedTerritories.getTerritories();

        cout << "Player " << playerName << " can defend: ";

        if(ownedTerrs.empty()) {

            cout << "no territories\n";

        } else {

            for(size_t i = 0; i < ownedTerrs.size(); i++) {

                cout << ownedTerrs[i] -> getID() << " (" << ownedTerrs[i]->getNumArmies() << " armies)";
                    
                if(i < ownedTerrs.size() - 1){ cout << ", "; } 
                
            }

            cout << "\n";

        }

    }

    void Player::issueOrders() {
        
        //TO BE DONE

    }

    void Player::addOwnedTerritories(Territory* territory) { ownedTerritories.addTerritory(territory, this); }

    void Player::removeOwnedTerritories(Territory* territory) { ownedTerritories.removeTerritory(territory); }

    void Player::addNeutralEnemy(const string& enemyName) {

        if(find(neutralEnemies.begin(), neutralEnemies.end(), enemyName) == neutralEnemies.end()) {
            neutralEnemies.push_back(enemyName); 
        }
    
    }

    void Player::removeNeutralEnemy(const string& enemyName) {

        auto enemyNameIndex = find(neutralEnemies.begin(), neutralEnemies.end(), enemyName);

        if (enemyNameIndex != neutralEnemies.end()) {
            neutralEnemies.erase(enemyNameIndex); 
        } 

    }

    void Player::clearNeutralEnemies() {

        neutralEnemies.clear();

    }
    
}
