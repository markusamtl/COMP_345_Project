#include "Order.h"

namespace WarzoneOrder {

    /*------------------------------------------ ORDER SUPER-CLASS ------------------------------------------------*/
    // ================= Order (base class) ================= //

    //-- Constructors, Destructor, Copy Constructor, Assignment Operator, Stream Insertion Operator --//

    Order::Order() {

        //No need to define the order type for default constructor. Calling the relevant subclasses will take care of this
        this -> effect = "";

    }

    Order::Order(OrderType t) {

        this -> orderType = t;
        this -> effect = "";

    }

    Order::~Order(){} //No dynamically loaded memory is implemented

    Order::Order(OrderType t, string effect) {

        this -> orderType = t;
        this -> effect = effect;

    }

    Order::Order(const Order& other) {

        this -> orderType = other.orderType;
        this -> effect = other.effect;

    }

    Order& Order::operator=(const Order& other) {

        if(this != &other) {

            this -> orderType = other.orderType;
            this -> effect = other.effect;

        }

        return *this;

    }

    ostream& operator<<(std::ostream& os, const Order& order) {

        order.print(os); //Print is virtual, such that different orders print different results
        return os;

    }

    void Order::print(std::ostream& os) const {

        string typeName;

        switch (orderType) {

            case (OrderType::Deploy):    typeName = "Deploy"; break;
            case (OrderType::Advance):   typeName = "Advance"; break;
            case (OrderType::Bomb):      typeName = "Bomb"; break;
            case (OrderType::Blockade):  typeName = "Blockade"; break;
            case (OrderType::Airlift):   typeName = "Airlift"; break;
            case (OrderType::Negotiate): typeName = "Negotiate"; break;
            default:                   typeName = "Unknown"; break;

        }

        os << "Order(Type: " << typeName << ", Effect: " << (effect.empty() ? "None" : effect) << ")";

    }


    //-- Accessors and Mutators --//

    OrderType Order::getOrderType() const { return this -> orderType; }

    void Order::setOrderType(OrderType t) { this -> orderType = t; }

    const string& Order::getEffect() const { return this -> effect; }

    void Order::setEffect(const string& e) { this -> effect = e; }

    //-- Interface Class Methods (Defined in the header, no need for implementation) --//


    /*------------------------------------------ SINGLE ORDER SUB-CLASSES ------------------------------------------------*/
    // ================= Deploy ================= //

    //-- Constructors, Destructor, Copy Constructor, Assignment Operator, Stream Insertion Operator --//

    Deploy::Deploy(Player* p, Territory* t, int armies) {

        this -> orderType = OrderType::Deploy;
        this -> issuer = p;
        this -> target = t;
        this -> numArmies = armies;
        this -> effect = "";

    }

    //When order is deleted, it should NOT remove any other pointers, since they still exist
    Deploy::~Deploy() = default;

    Deploy::Deploy(const Deploy& other) : Order(other) {

        this -> issuer = other.issuer;
        this -> target = other.target;
        this -> numArmies = other.numArmies;

    }

    Deploy& Deploy::operator=(const Deploy& other) {

        if(this != &other) {

            Order::operator=(other);
            this -> issuer = other.issuer;
            this -> target = other.target;
            this -> numArmies = other.numArmies;

        }

        return *this;

    }

    void Deploy::print(std::ostream& os) const {

        os << "Deploy Order: "
           << (issuer ? issuer -> getPlayerName() : "Unknown Player") //Get player name
           << " deploys to " << (target ? target -> getID() : "Unknown Territory") //Get target territory name
           << " with " << numArmies
           << " armies. Effect: " << (effect.empty() ? "None" : effect); //Get effect

    }

    //-- Accessors and Mutators --//

    Player* Deploy::getIssuer() const { return issuer; }
    void Deploy::setIssuer(Player* p) { issuer = p; }

    Territory* Deploy::getTarget() const { return target; }
    void Deploy::setTarget(Territory* t) { target = t; }

    int Deploy::getNumArmies() const { return numArmies; }
    void Deploy::setNumArmies(int armies) { numArmies = armies; }

    //-- Overridden Class Methods --//

    Order* Deploy::clone() const { return new Deploy(*this); }

    bool Deploy::validate() const {
        return false; // TODO: implement
    }

    void Deploy::execute() {
        // TODO: implement
    }

    // ================= Advance ================= //

    //-- Constructors, Destructor, Copy Constructor, Assignment Operator, Stream Insertion Operator --//

    Advance::Advance(Player* p, Territory* s, Territory* t, int armies) {

        this -> orderType = OrderType::Advance;
        this -> issuer = p;
        this -> source = s;
        this -> target = t;
        this -> numArmies = armies;
        this -> effect = "";

    }

    //When order is deleted, it should NOT remove any other pointers, since they still exist
    Advance::~Advance() = default;

    Advance::Advance(const Advance& other) : Order(other) {

        this -> issuer = other.issuer;
        this -> source = other.source;
        this -> target = other.target;
        this -> numArmies = other.numArmies;

    }

    Advance& Advance::operator=(const Advance& other) {

        if(this != &other) {

            Order::operator=(other);
            this -> issuer = other.issuer;
            this -> source = other.source;
            this -> target = other.target;
            this -> numArmies = other.numArmies;

        }

        return *this;

    }

    void Advance::print(std::ostream& os) const {

        os << "Advance Order: "
           << (issuer ? issuer -> getPlayerName() : "Unknown Player") //Get player name
           << " moves " << numArmies << " armies " //Get number of armies
           << "from " << (source ? source -> getID() : "Unknown Source") //Get source territory
           << " to " << (target ? target -> getID() : "Unknown Target") //Get target territory
           << ". Effect: " << (effect.empty() ? "None" : effect); //Get effect

    }

    //-- Accessors and Mutators --//

    Player* Advance::getIssuer() const { return issuer; }
    void Advance::setIssuer(Player* p) { issuer = p; }

    Territory* Advance::getSource() const { return source; }
    void Advance::setSource(Territory* s) { source = s; }

    Territory* Advance::getTarget() const { return target; }
    void Advance::setTarget(Territory* t) { target = t; }

    int Advance::getNumArmies() const { return numArmies; }
    void Advance::setNumArmies(int armies) { numArmies = armies; }

    //-- Overridden Class Methods --//

    Order* Advance::clone() const { return new Advance(*this); }

    bool Advance::validate() const {
        return false; // TODO: implement
    }

    void Advance::execute() {
        // TODO: implement
    }



    // ================= Bomb ================= //

    //-- Constructors, Destructor, Copy Constructor, Assignment Operator, Stream Insertion Operator --//

    Bomb::Bomb(Player* p, Territory* t) {

        this -> orderType = OrderType::Bomb;
        this -> issuer = p;
        this -> target = t;
        this -> effect = "";

    }

    //When order is deleted, it should NOT remove any other pointers, since they still exist
    Bomb::~Bomb() = default; 

    Bomb::Bomb(const Bomb& other) : Order(other) {

        this -> issuer = other.issuer;
        this -> target = other.target;

    }

    Bomb& Bomb::operator=(const Bomb& other) {

        if(this != &other) {

            Order::operator=(other);
            this -> issuer = other.issuer;
            this -> target = other.target;

        }

        return *this;

    }

    void Bomb::print(std::ostream& os) const {

        os << "Bomb Order: "
           << (issuer ? issuer -> getPlayerName() : "Unknown Player") //Get player name
           << " bombs " << (target ? target -> getID() : "Unknown Territory") //Get target territory name
           << ". Effect: " << (effect.empty() ? "None" : effect); //Get effect

    }

    //-- Accessors and Mutators --//

    Player* Bomb::getIssuer() const { return issuer; }
    void Bomb::setIssuer(Player* p) { issuer = p; }

    Territory* Bomb::getTarget() const { return target; }
    void Bomb::setTarget(Territory* t) { target = t; }

    //-- Overridden Class Methods --//

    Order* Bomb::clone() const { return new Bomb(*this); }

    bool Bomb::validate() const {
        return false; // TODO: implement
    }

    void Bomb::execute() {
        // TODO: implement
    }



    // ================= Blockade ================= //

    //-- Constructors, Destructor, Copy Constructor, Assignment Operator, Stream Insertion Operator --//

    Blockade::Blockade(Player* p, Territory* t, Player* neutral) {

        this -> orderType = OrderType::Blockade;
        this -> issuer = p;
        this -> target = t;
        this -> neutralPlayer = neutral;
        this -> effect = "";

    }

    //When order is deleted, it should NOT remove any other pointers, since they still exist
    Blockade::~Blockade() = default;

    Blockade::Blockade(const Blockade& other) : Order(other) {

        this -> issuer = other.issuer;
        this -> target = other.target;
        this -> neutralPlayer = other.neutralPlayer;

    }

    Blockade& Blockade::operator=(const Blockade& other) {

        if(this != &other) {

            Order::operator=(other);
            this -> issuer = other.issuer;
            this -> target = other.target;
            this -> neutralPlayer = other.neutralPlayer;

        }

        return *this;

    }

    void Blockade::print(std::ostream& os) const {

        os << "Blockade Order: "
           << (issuer ? issuer -> getPlayerName() : "Unknown Player") //Get player name
           << " blockades " << (target ? target -> getID() : "Unknown Territory") //Get target territory
           << ". Territory is transferred to a neutral player, " 
           << (neutralPlayer ? neutralPlayer -> getPlayerName() : "UNKNOWN NAME") //Get neutral player
           << ". Effect: " << (effect.empty() ? "None" : effect); //Get effect

    }

    //-- Accessors and Mutators --//

    Player* Blockade::getIssuer() const { return issuer; }
    void Blockade::setIssuer(Player* p) { issuer = p; }

    Territory* Blockade::getTarget() const { return target; }
    void Blockade::setTarget(Territory* t) { target = t; }

    Player* Blockade::getNeutralPlayer() const { return neutralPlayer; }
    void Blockade::setNeutralPlayer(Player* neutral) { neutralPlayer = neutral; }

    //-- Overridden Class Methods --//

    Order* Blockade::clone() const { return new Blockade(*this); }

    bool Blockade::validate() const {
        return false; // TODO: implement
    }

    void Blockade::execute() {
        // TODO: implement
    }



    // ================= Airlift ================= //

    //-- Constructors, Destructor, Copy Constructor, Assignment Operator, Stream Insertion Operator --//

    Airlift::Airlift(Player* p, Territory* s, Territory* t, int armies) {

        this -> orderType = OrderType::Airlift;
        this -> issuer = p;
        this -> source = s;
        this -> target = t;
        this -> numArmies = armies;
        this -> effect = "";

    }

    //When order is deleted, it should NOT remove any other pointers, since they still exist
    Airlift::~Airlift() = default;

    Airlift::Airlift(const Airlift& other) : Order(other) {

        this -> issuer = other.issuer;
        this -> source = other.source;
        this -> target = other.target;
        this -> numArmies = other.numArmies;

    }

    Airlift& Airlift::operator=(const Airlift& other) {

        if(this != &other) {

            Order::operator=(other);
            this -> issuer = other.issuer;
            this -> source = other.source;
            this -> target = other.target;
            this -> numArmies = other.numArmies;

        }

        return *this;

    }

    void Airlift::print(std::ostream& os) const {

        os << "Airlift Order: "
           << (issuer ? issuer -> getPlayerName() : "Unknown Player") //Get player name
           << " airlifts " << numArmies << " armies " //Get number of armies
           << "from " << (source ? source -> getID() : "Unknown Source") //Get source territory
           << " to " << (target ? target -> getID() : "Unknown Target") //Get target territory
           << ". Effect: " << (effect.empty() ? "None" : effect); //Get effect

    }

    //-- Accessors and Mutators --//

    Player* Airlift::getIssuer() const { return issuer; }
    void Airlift::setIssuer(Player* p) { issuer = p; }

    Territory* Airlift::getSource() const { return source; }
    void Airlift::setSource(Territory* s) { source = s; }

    Territory* Airlift::getTarget() const { return target; }
    void Airlift::setTarget(Territory* t) { target = t; }

    int Airlift::getNumArmies() const { return numArmies; }
    void Airlift::setNumArmies(int armies) { numArmies = armies; }

    //-- Overridden Class Methods --//

    Order* Airlift::clone() const { return new Airlift(*this); }

    bool Airlift::validate() const {
        return false; // TODO: implement
    }

    void Airlift::execute() {
        // TODO: implement
    }



    // ================= Negotiate ================= //

    //-- Constructors, Destructor, Copy Constructor, Assignment Operator, Stream Insertion Operator --//

    Negotiate::Negotiate(Player* p, Player* other) {

        this -> orderType = OrderType::Negotiate;
        this -> issuer = p;
        this -> targetPlayer = other;
        this -> effect = "";

    }

    //When order is deleted, it should NOT remove any other pointers, since they still exist
    Negotiate::~Negotiate() = default;

    Negotiate::Negotiate(const Negotiate& other) : Order(other) {

        this -> issuer = other.issuer;
        this -> targetPlayer = other.targetPlayer;

    }

    Negotiate& Negotiate::operator=(const Negotiate& other) {

        if(this != &other) {

            Order::operator=(other);
            this -> issuer = other.issuer;
            this -> targetPlayer = other.targetPlayer;

        }

        return *this;

    }

    void Negotiate::print(std::ostream& os) const {

        os << "Negotiate Order: "
           << (issuer ? issuer -> getPlayerName() : "Unknown Player") //Get issuing player
           << " negotiates peace with " 
           << (targetPlayer ? targetPlayer -> getPlayerName() : "Unknown Player") //Get target player
           << ". Effect: " << (effect.empty() ? "None" : effect); //Get effect

    }

    //-- Accessors and Mutators --//

    Player* Negotiate::getIssuer() const { return issuer; }
    void Negotiate::setIssuer(Player* p) { issuer = p; }

    Player* Negotiate::getTargetPlayer() const { return targetPlayer; }
    void Negotiate::setTargetPlayer(Player* other) { targetPlayer = other; }

    //-- Overridden Class Methods --//

    Order* Negotiate::clone() const { return new Negotiate(*this); }

    bool Negotiate::validate() const {
        return false; // TODO: implement
    }

    void Negotiate::execute() {
        // TODO: implement
    }

    // ================= OrderList ================= //

    //-- Constructors, Destructor, Copy Constructor, Assignment Operator, Stream Insertion Operator --//

    OrderList::OrderList() {

        this -> orders = new std::vector<Order*>(); //Initialize empty container

    }

    OrderList::~OrderList() {

        for(Order* o : *orders) { delete o; } //Delete all orders in the list

        delete orders;

    }

    OrderList::OrderList(const OrderList& other) {

        this -> orders = new std::vector<Order*>();

        //Deep copy using clone()
        for(Order* o : *other.orders) {

            this -> orders -> push_back(o -> clone());

        }

    }

    OrderList& OrderList::operator=(const OrderList& other) {

        if(this != &other) {

            for(Order* o : *this -> orders) { delete o; } //Delete existing orders in the list
            delete this -> orders;

            //Rebuild container with deep copy
            this -> orders = new std::vector<Order*>();

            for(Order* o : *other.orders) {

                this -> orders -> push_back(o -> clone());

            }

        }

        return *this;

    }

    std::ostream& operator<<(std::ostream& os, const OrderList& ol) {

        os << "OrderList: " << std::endl;

        for(size_t i = 0; i < ol.orders -> size(); i++) { //Iterate over all orders

            os << "  [" << i << "] " << *ol.orders -> at(i) << std::endl; //This WILL print the correct order output based on class

        }

        return os;

    }

    //-- Accessors and Mutators --//

    std::vector<Order*>* OrderList::getOrders() const { return orders; }

    void OrderList::setOrders(const std::vector<Order*>* newOrders) { 
    
        // Clean up existing orders
        for(Order* o : *orders) { delete o; }
        delete orders;

        // Allocate a new vector and clone each order
        this -> orders = new std::vector<Order*>();

        for(Order* o : *newOrders) {

            this -> orders -> push_back(o -> clone()); 

        }

    }


    //-- Class Methods --//

    void OrderList::addOrder(Order* o) {

        this -> orders -> push_back(o);

    }

    void OrderList::remove(int index) {

        /**
         * This method is a bit tricky to understand, so a visual demo will be provided:
         * Let ordVec = [A, B, C, D, E]
         * ordVec executes as follows, for method call move(1)
         * Step 1: delete B (free its memory, since OrderList owns its Orders).
         * Step 2: vector becomes [A, C, D, E] (erase shifts all remaining elements left).
         * 
         * Note: the vector only removes the pointer slot;
         * explicit delete is required to release the object itself.
         */

        //Check if the index is greater than 0, or less than the size of the orders vector
        if(index >= 0 && index < static_cast<int>(this -> orders -> size())) {

            delete this -> orders -> at(index); //Free memory of removed order (prevent memory leak)

            //Shift all elements after the deleted element to the left by 1 index
            this -> orders -> erase(this -> orders -> begin() + index);

        }

    }

    void OrderList::move(int oldPos, int newPos) {

        /**
         * This method is a bit tricky to understand, so a visual demo will be provided:
         * Let ordVec = [A, B, C, D, E]
         * ordVec executes as follows, for method call move(0, 1)
         * Step 1: Order = A
         * Step 2: ordVec = [B, C, D, E]
         * Step 3: ordVec = [B, A, C, D, E]
         * inserted here        ^ 
        */

        //Make sure inputs are IN BOUNDS of the list
        bool geqZeroOldPos = oldPos >= 0;
        bool leqListSizeOldPos = oldPos < static_cast<int>(this -> orders -> size());
        bool geqZeroNewPos = newPos >= 0;
        bool leqListSizeNewPos = newPos < static_cast<int>(this -> orders -> size());

        if(geqZeroOldPos && leqListSizeOldPos && geqZeroNewPos && leqListSizeNewPos){

            //Take out the order
            Order* tempOrder = this -> orders -> at(oldPos); //Get order pointer

            //Shift all elements after the deleted element to the left by 1 index
            this -> orders -> erase(this -> orders -> begin() + oldPos); 

            //Reinsert at new position
            this -> orders -> insert(this -> orders -> begin() + newPos, tempOrder);

        }

    }

}