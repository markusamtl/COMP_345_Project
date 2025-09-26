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
        territories.clear();
        territoryIndex.clear();
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
        for (size_t i = 0; i < pc.territories.size(); i++) {
            os << pc.territories[i]->getID();
            if (i < pc.territories.size() - 1) os << ", ";
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
        for (Territory* t : territories) {
            if (t != nullptr) territoryIndex[t->getID()] = t;
        }
    }

    const unordered_map<string, Territory*>& PlayerTerrContainer::getTerritoryIndex() const {
        return territoryIndex;
    }

    void PlayerTerrContainer::setTerritoryIndex(const unordered_map<string, Territory*>& newIndex) {
        territoryIndex = newIndex; //Overwrite old hashmap
        territories.clear();
        for (const pair<const string, Territory*>& kv : territoryIndex) {
            territories.push_back(kv.second);
        }
    }

    //-- Class Methods --//

    void PlayerTerrContainer::addTerritory(Territory* t) {
        if (this->owns(t)) return; //Check if null or already exists
        territories.push_back(t);
        territoryIndex[t->getID()] = t;
    }

    void PlayerTerrContainer::removeTerritory(Territory* t) {
        if (!(this->owns(t))) return; //Check if null or doesn’t exist
        territoryIndex.erase(territoryIndex.find(t->getID()));
        territories.erase(remove(territories.begin(), territories.end(), t), territories.end());
    }

    bool PlayerTerrContainer::owns(Territory* t) const {
        if (t == nullptr) return false;
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

    Player::Player(const string& name, Hand* hand, OrderList* orders) {

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
      this -> playerOrders = (other.playerOrders ? new OrderList(*other.playerOrders) : nullptr); //Check if other.playerOrders is a nullptr or not

  }

  Player& Player::operator=(const Player& other) {

      if (this != &other) {

          this->playerName = other.playerName;
          this->ownedTerritories = other.ownedTerritories;

          //Remove, since this is a reassignment.
          delete this->playerHand;
          delete this->playerOrders;

          this->playerHand = (other.playerHand ? new Hand(*other.playerHand) : nullptr);  //Check if other.playerHand is a nullptr or not
          this->playerOrders = (other.playerOrders ? new OrderList(*other.playerOrders) : nullptr); //Check if other.playerOrders is a nullptr or not


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

      if(!(playerHand == nullptr)){ delete playerHand; }
      playerHand = newHand;

  }

  OrderList* Player::getOrders() const { return playerOrders; }

  void Player::setOrders(OrderList* newOrders) {

      if(playerOrders){ delete playerOrders; }
      playerOrders = newOrders;

  }

  //-- Class Methods --//

  vector<WarzoneMap::Territory*> Player::toAttack() {

      vector<WarzoneMap::Territory*> retArr;

      for(WarzoneMap::Territory* currTerr : this -> ownedTerritories.getTerritories()){

          cout << "From: " << currTerr -> getID() << ", " << this -> getPlayerName() << " can attack: ";



      }

      return retArr;
  }

  vector<WarzoneMap::Territory*> Player::toDefend() {
      // Placeholder: just return all owned territories
      return ownedTerritories.getTerritories();
  }

  void Player::issueOrders() {
      // Placeholder: order creation logic goes here
      if (playerOrders) {
          // Example: playerOrders->addOrder(new DeployOrder(...));
      }
  }

  void Player::addOwnedTerritories(WarzoneMap::Territory* territory) { ownedTerritories.addTerritory(territory); }

  void Player::removeOwnedTerritories(WarzoneMap::Territory* territory) { ownedTerritories.removeTerritory(territory); }

}