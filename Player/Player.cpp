#include "Player.h"

//-- Constructors, Destructor, Copy Constructor, Assignment Operator, Stream Insertion Operator --//

#include "Player.h"

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