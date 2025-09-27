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
        
        this->territories = territories;
        this->territoryIndex = territoryIndex;
    
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

    void PlayerTerrContainer::setTerritories(const vector<Territory*>& newTerritories) {

        territories = newTerritories; //Overwrite old territory vector 
        territoryIndex.clear();       //Clear old hashmap

        for(Territory* t : territories) {
        
            if(t != nullptr){ territoryIndex[t->getID()] = t; }
        
        }

    }

    const unordered_map<string, Territory*>& PlayerTerrContainer::getTerritoryIndex() const {

        return territoryIndex;
    
    }

    void PlayerTerrContainer::setTerritoryIndex(const unordered_map<string, Territory*>& newIndex) {
        
        territoryIndex = newIndex; //Overwrite old hashmap
        territories.clear();

        for (const pair<const string, Territory*>& currTerr : territoryIndex) {

            territories.push_back(currTerr.second);

        }

    }

    //-- Class Methods --//
    void PlayerTerrContainer::addTerritory(Territory* t) {

        if(this -> owns(t)){ return; } //Check if null or already exists
        
        territories.push_back(t);
        territoryIndex[t -> getID()] = t;

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
        this -> generateCardThisTurn = false; // default

    }

    Player::Player(const string& name) {

        this -> playerName = name;
        this -> neutralEnemies = {};
        this -> ownedTerritories = PlayerTerrContainer();
        this -> playerHand = nullptr;
        this -> playerOrders = nullptr;
        this -> generateCardThisTurn = false; // default

    }

    Player::Player(const string& name, Hand* hand, OrderList* orders) {

        this -> playerName = name;
        this -> neutralEnemies = {};
        this -> ownedTerritories = PlayerTerrContainer();
        this -> playerHand = hand;
        this -> playerOrders = orders;
        this -> generateCardThisTurn = false; // default

    }

    Player::Player(const string& name, vector<string> neutralEnemies, PlayerTerrContainer ownedTerritories, Hand* hand, OrderList* orders, bool generateCard) {

        this -> playerName = name;
        this -> neutralEnemies = neutralEnemies;
        this -> ownedTerritories = ownedTerritories;
        this -> playerHand = hand;
        this -> playerOrders = orders;
        this -> generateCardThisTurn = generateCard; // passed in explicitly

    }

    Player::~Player() { 

        //Since the hand and orders are no longer needed
        delete this -> playerHand; 
        delete this -> playerOrders;

    }

    Player::Player(const Player& other) {

        this -> playerName = other.playerName;
        this -> neutralEnemies = other.neutralEnemies;
        this -> ownedTerritories = other.ownedTerritories;
        this -> playerHand = (other.playerHand ? new Hand(*other.playerHand) : nullptr);
        this -> playerOrders = (other.playerOrders ? new OrderList(*other.playerOrders) : nullptr);
        this -> generateCardThisTurn = other.generateCardThisTurn; // copy flag

    }

    Player& Player::operator=(const Player& other) {

        if (this != &other) {

            delete this -> playerHand;
            delete this -> playerOrders;

            this -> playerName = other.playerName;
            this -> neutralEnemies = other.neutralEnemies;
            this -> ownedTerritories = other.ownedTerritories;
            this -> playerHand = (other.playerHand ? new Hand(*other.playerHand) : nullptr);
            this -> playerOrders = (other.playerOrders ? new OrderList(*other.playerOrders) : nullptr);
            this -> generateCardThisTurn = other.generateCardThisTurn; // copy flag

        }

        return *this;

    }

    ostream& operator<<(ostream& os, const Player& p) {

        os << "Player(Name: " << p.playerName
           << ", Territories: " << p.ownedTerritories.size()
           << ", Hand: " << (p.playerHand ? "Yes" : "No")
           << ", Orders: " << (p.playerOrders ? "Yes" : "No")
           << ", GenerateCardThisTurn: " << (p.generateCardThisTurn ? "True" : "False")
           << ")";
        return os;

    }

    //-- Accessors and Mutators --//

    const string& Player::getPlayerName() const { return playerName; }

    void Player::setPlayerName(const string& name) { playerName = name; }

    const vector<string>& Player::getNeutralEnemies() const { return neutralEnemies; }

    void Player::setNeutralEnemies(const vector<string>& enemies) { neutralEnemies = enemies; }

    const PlayerTerrContainer& Player::getOwnedTerritories() const { return ownedTerritories; }

    void Player::setOwnedTerritories(const PlayerTerrContainer& newTerritories) { ownedTerritories = newTerritories; }

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

                string neighOwner = neighTerr -> getOwner();

                // Skip if owned by self or neutral enemy
                if(neighOwner == playerName || 
                    find(neutralEnemies.begin(), neutralEnemies.end(), neighOwner) 
                    != neutralEnemies.end()) { continue; }

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

                string neighOwner = neighTerr -> getOwner();

                if (neighOwner == playerName ||
                    std::find(neutralEnemies.begin(), neutralEnemies.end(), neighOwner)
                    != neutralEnemies.end()) { continue; }

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

    void Player::addOwnedTerritories(Territory* territory) { ownedTerritories.addTerritory(territory); }

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
