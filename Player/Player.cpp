#include "Player.h"

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
        this -> ownedTerritories = PlayerTerrContainer();
        this -> playerHand = nullptr;
        this -> playerOrders = nullptr;

    }

    Player::Player(const string& name) {

        this -> playerName = name;
        this -> ownedTerritories = PlayerTerrContainer();
        this -> playerHand = nullptr;
        this -> playerOrders = nullptr;

    }

    Player::Player(const string& name, Hand* hand, Order* orders) {

        this -> playerName = name;
        this -> ownedTerritories = PlayerTerrContainer();
        this -> playerHand = hand;
        this -> playerOrders = orders;

    }

    Player::~Player() {

        delete playerHand;
        delete playerOrders;

    }

  Player::Player(const Player& other) {

      this -> playerName = other.playerName;
      this -> ownedTerritories = other.ownedTerritories;
      this -> playerHand = (other.playerHand ? new Hand(*other.playerHand) : nullptr); //Check if other.playerHand is a nullptr or not
      this -> playerOrders = (other.playerOrders ? new Order(*other.playerOrders) : nullptr); //Check if other.playerOrders is a nullptr or not

  }

  Player& Player::operator=(const Player& other) {

      if (this != &other) {

          this->playerName = other.playerName;
          this->ownedTerritories = other.ownedTerritories;

          //Remove, since this method performs a reassignment.
          delete this->playerHand;
          delete this->playerOrders;

          this->playerHand = (other.playerHand ? new Hand(*other.playerHand) : nullptr);  //Check if other.playerHand is a nullptr or not
          this->playerOrders = (other.playerOrders ? new Order(*other.playerOrders) : nullptr); //Check if other.playerOrders is a nullptr or not


      }

      return *this;

  }


  ostream& operator<<(ostream& os, const Player& p) {

      os << "Player(Name: " << p.playerName 
         << ", Territories Owned: " << p.ownedTerritories.size() 
         << ", Hand: " << (p.playerHand ? "Present" : "None")
         << ", Orders: " << (p.playerOrders ? "Present" : "None") 
         << ")";

      return os;

  }

  //-- Accessors and Mutators --//

  const string& Player::getPlayerName() const { return playerName; }

  void Player::setPlayerName(const string& name) { playerName = name; }

  const PlayerTerrContainer& Player::getOwnedTerritories() const { return ownedTerritories; }

  void Player::setOwnedTerritories(const PlayerTerrContainer& newTerritories) { ownedTerritories = newTerritories; }

  Hand* Player::getHand() const { return playerHand; }

  void Player::setHand(Hand* newHand) {

      if(!(playerHand == nullptr)){ delete playerHand; } //Clear memory. Change later maybe?
      playerHand = newHand;

  }

  Order* Player::getOrders() const { return playerOrders; }

  void Player::setOrders(Order* newOrders) {

      if(playerOrders != nullptr){ delete playerOrders; } //Clear memory. Change later maybe?
      playerOrders = newOrders;

  }

  //-- Class Methods --//

    vector<Territory*> Player::toAttack() {

        unordered_set<Territory*> uniqueTargets; //Unordered set to avoid duplicate attack targets

        for(Territory* currTerr : ownedTerritories.getTerritories()) {

            cout << "From: " << currTerr -> getID()  << ", " << playerName << " can attack: ";

            bool foundEnemy = false; //Helpful if a player's territory is surrounded by friendly territory

            for(Territory* neighTerr : currTerr -> getNeighbors()) {

                if(neighTerr -> getOwner() != playerName) {

                    // Print all valid neighbors, even if duplicate
                    cout << neighTerr -> getID() << " (" << neighTerr ->getNumArmies() << " armies), ";
                    foundEnemy = true;

                    // Insert into set (since it's unordered, discard duplicate territories)
                    uniqueTargets.insert(neighTerr);
                }

            }

            if(foundEnemy == false) { //Indicates that from this territory, there are no enemy neighbours

                cout << "no enemies";

            }

            cout << "\n";

        }

        // Convert the set into a vector for the return type
        vector<Territory*> validNeighbourArray(uniqueTargets.begin(), uniqueTargets.end());
        return validNeighbourArray;

    }


    vector<Territory*> Player::toDefend() {

        vector<Territory*> ownedTerrs = ownedTerritories.getTerritories();

        cout << "Player " << playerName << " can defend: ";

        if(ownedTerrs.empty()) {

            cout << "no territories\n";

        } else {

            for(size_t i = 0; i < ownedTerrs.size(); i++) {

                cout << ownedTerrs[i] -> getID() << " (" << ownedTerrs[i]->getNumArmies() << " armies)";
                
                if(i < ownedTerrs.size() - 1){ cout << ", "; } //Make sure no extra commas are added 
            }

            cout << "\n";

        }

        return ownedTerrs;
        
    }


  void Player::issueOrders() {
      
    //TO BE DONE

  }

  void Player::addOwnedTerritories(Territory* territory) { ownedTerritories.addTerritory(territory); }

  void Player::removeOwnedTerritories(Territory* territory) { ownedTerritories.removeTerritory(territory); }

}