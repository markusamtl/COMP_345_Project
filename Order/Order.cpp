#include "Order.h"

namespace WarzoneOrder {

    /*------------------------------------------TIME UTILITY CLASS--------------------------------------------------*/

    double TimeUtil::getSystemTimeSeconds() {

        using namespace std::chrono;

        auto now = high_resolution_clock::now();
        auto duration = now.time_since_epoch();
        return duration_cast<chrono::duration<double>>(duration).count();

    }

    long long TimeUtil::getSystemTimeMillis() {

        using namespace std::chrono;
        auto now = high_resolution_clock::now();
        auto duration = now.time_since_epoch();
        return duration_cast<milliseconds>(duration).count();

    }

    long long TimeUtil::getSystemTimeNano() {

        using namespace std::chrono;
        auto now = high_resolution_clock::now();
        auto duration = now.time_since_epoch();
        return duration_cast<nanoseconds>(duration).count();
        
    }

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

        //Check if either the territory or the player are null pointers
        if(issuer == nullptr || target == nullptr){ return false; } 

        // Territory must belong to the issuing player
        if(target -> getOwner() != issuer){ return false; }

        // Army count must be positive
        if(numArmies <= 0){ return false; }

        return true; //Return true if all conditions are met

    }

    void Deploy::execute() {

        if (!this->validate()) {

            this->setEffect("Deploy order invalid.");
            cout << this->getEffect() << endl;
            return;

        }

        // Add armies to target
        target->setNumArmies(target->getNumArmies() + numArmies);

        // --- Safeguard: ensure target is tracked by issuer ---
        if (!issuer->getOwnedTerritories().owns(target)) { issuer->addOwnedTerritories(target); }

        this->setEffect("Deploy successful: placed " + to_string(numArmies) +
                        " armies on " + target->getID() + ".");

        cout << this->getEffect() << endl;

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

        //Check if either of the territories, or the player, are null pointers
        if(issuer == nullptr || source == nullptr || target == nullptr) { return false; }

        //Player must own the source territory
        if(source -> getOwner() != issuer){ return false; }

        //Player must have enough armies, or the number of armies they use must be geq 0
        if(numArmies <= 0 || source -> getNumArmies() < numArmies) { return false; }

        // Any advance order requires the target to be adjacent to the source
        vector<Territory*> neighbors = source -> getNeighbors();

        //Check if the target territory is a neighbour of the source territory
        bool isAdjacent = (find(neighbors.begin(), neighbors.end(), target) != neighbors.end());
        if(!isAdjacent) { return false; }

        //Check if the attacker has a truce with the defender
        Player* defender = target -> getOwner();
        
        if (defender != nullptr) {

            const vector<string>& neutrals = issuer->getNeutralEnemies();

            if(find(neutrals.begin(), neutrals.end(), defender->getPlayerName()) != neutrals.end()) { //Search through list of neutral enemies

                return false;
            
            }
        
        }

        return true;
        
    }

    void Advance::execute() {

        // --- 1. Validate ownership and adjacency ---
        if(!this -> validate()) {

            this -> setEffect("Advance order invalid.");
            cout << this -> getEffect() << endl;
            return;

        }

        // --- 2. Check if source belongs to issuer ---
        if(source -> getOwner() != issuer) {

            this -> setEffect("Advance failed: source territory not owned by issuer.");
            cout << this -> getEffect() << endl;
            return;

        }

        // --- 3. Check adjacency ---
        const vector<Territory*>& neighbors = source -> getNeighbors();

        //Check if the target is in the neighbour vector
        if(find(neighbors.begin(), neighbors.end(), target) == neighbors.end()){

            this -> setEffect("Advance failed: target territory is not adjacent.");
            cout << this -> getEffect() << endl;
            return;

        }

        // --- 4. If target is also owned by issuer, reinforce ---
        if (target->getOwner() == issuer) {

            if (source->getNumArmies() <= 1) { //1 Army MUST remain on the source 

                this->setEffect("Advance failed: Not enough armies to advance with.");
                cout << this->getEffect() << endl;
                return;
                
            }

            int numArmiesMovable = min(numArmies, source->getNumArmies() - 1); //Ensure 1 army stays on source

            source->setNumArmies(source->getNumArmies() - numArmiesMovable); // Remove Armies from source
            target->setNumArmies(target->getNumArmies() + numArmiesMovable); // Add armies to target

            //Ensure both territories are tracked by issuer ---
            if (!issuer -> getOwnedTerritories().owns(source)) { issuer -> addOwnedTerritories(source); }
            if (!issuer->getOwnedTerritories().owns(target)) { issuer->addOwnedTerritories(target); }

            this->setEffect("Advance successful: moved " + to_string(numArmiesMovable) +
                            " armies from " + source -> getID() +
                            " to " + target -> getID() + ".");

            cout << this->getEffect() << endl;

            return;
        
        }


        // --- 5. Perform battle logic
        if(source -> getNumArmies() <= 1) { //1 Army MUST remain on the source 

            this -> setEffect("Advance failed: Not enough armies to attack with.");
            cout << this -> getEffect() << endl;
            return;

        }

        int attackArmies = min(numArmies, source -> getNumArmies() - 1); //Ensure 1 army stays on source
        int defenseArmies = target -> getNumArmies();

        //Remove armies from source no matter what
        source -> setNumArmies(source -> getNumArmies() - attackArmies);

        int attackersRemaining = attackArmies;
        int defendersRemaining = defenseArmies;

        //Boolean flags to help determine the result of battle-turns
        bool reduceAttacker = false;
        bool reduceDefense = false;

        /**
         * Simultaneous battle: attackers have 60% chance kill a defender, defenders have a 70% chance to kill an attacker
         * Attacker has to have 1 army on their source territory at a minimum
         */
         while(attackersRemaining > 1 && defendersRemaining > 0){ 

            reduceAttacker = false;
            reduceDefense = false;

            //Defender has 70% chance to kill attacking army
            if((static_cast<long long>(TimeUtil::getSystemTimeNano()) % 10000) < 7000) {
                
                reduceAttacker = true;

            }


            //Attacker has 60% chance to kill defending army
            if((static_cast<long long>(TimeUtil::getSystemTimeNano()) % 10000) < 6000){

                reduceDefense = true;

            }

            //Prevents attackers from being reduced to 1 in the case of attacker winning
            if(reduceAttacker  && attackersRemaining > 1){ attackersRemaining--; }
            if(reduceDefense && defendersRemaining > 0){ defendersRemaining--; }

        }

        //--- 6. Resolve outcome ---

        Player* oldOwner = target->getOwner(); //Get old conquered territory owner
        string oldOwnerName = (oldOwner ? oldOwner -> getPlayerName() : "Unknown Player");

        if (defendersRemaining <= 0) {

            if (attackersRemaining > 1) { // Successful conquest

                //Transfer ownership
                issuer->addOwnedTerritories(target);
                target->setNumArmies(attackersRemaining);

                issuer->setGenerateCardThisTurn(true); // Attacker earns a card at turn end

                this->setEffect("Advance battle successful: " + issuer->getPlayerName() +
                                " defeated " + oldOwnerName +
                                ", and conquered " + target->getID() + " with " +
                                to_string(attackersRemaining) + " surviving armies.");

            } else { // Edge case: attacker “wins” but can’t move in

                defendersRemaining = 1;
                target->setNumArmies(defendersRemaining);

                this->setEffect("Advance battle inconclusive: " +
                                issuer->getPlayerName() + " tried to beat " + oldOwnerName +
                                ", but didn't have enough armies to move in. The defender clings onto " + target->getID() +
                                " with 1 army, after a desperate defense.");

            }

        } else { // Defender Wins

            target -> setNumArmies(defendersRemaining);

            this->setEffect("Advance battle failed: defender " +
                            oldOwnerName + " beat " + issuer -> getPlayerName() + ", and holds onto"
                            + target -> getID() + " with " +
                            to_string(defendersRemaining) + " armies remaining.");

        }

        cout << this -> getEffect() << endl;

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

        //Make sure player and territory target aren't nullpointers 
        if(issuer == nullptr || target == nullptr) { return false; }

        //Target must NOT belong to the issuing player
        if(target -> getOwner() == issuer) { return false; }

        //Can't bomb a territory with only 1 army.
        if(target -> getNumArmies() == 1) { return false; }

        //Target must be adjacent to at least one of issuer's territories
        const auto& ownedTerritories = issuer -> getOwnedTerritories().getTerritories(); //Get player's territories

        for(Territory* terr : ownedTerritories) { //For all of the player's territories

            for (Territory* neighbor : terr->getNeighbors()) { //For each territory's neighbour

                if (neighbor == target) { return true; } //Check if the target territory is adjacent to player owned territory
            
            }

        }

        return false;

    }

    void Bomb::execute() {

        if(!validate()) {

            this -> setEffect("Bomb order invalid: cannot target this territory.");
            cout << this -> getEffect() << endl;
            return;

        }

        int currentArmies = target -> getNumArmies();
        int armiesToRemove = currentArmies / 2; // Remove half, rounded down
        
        target -> setNumArmies(currentArmies - armiesToRemove);

        this->setEffect("Bomb order executed. Player " + target -> getOwner() -> getPlayerName() + ", at territory " 
                        + target -> getID() + " lost " + to_string(armiesToRemove) + " armies.");

        cout << this->getEffect() << std::endl;

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

        //Check if the target territory, the issuing player, or the neutral player are null pointers
        if(issuer == nullptr || target == nullptr || neutralPlayer == nullptr) { return false; }

        //Target must belong to the issuing player
        if(target -> getOwner() != issuer) { return false; }

        return true;

    }


    void Blockade::execute() {

        if (!validate()) {

            this -> setEffect("Blockade order invalid: target territory does not belong to the issuer.");
            cout << this -> getEffect() << endl;
            return;

        }

        //Step 1: Triple the armies
        target -> setNumArmies((target -> getNumArmies()) * 3);

        //Step 2: Transfer ownership to Neutral player
        neutralPlayer -> addOwnedTerritories(target); //Add territory to neutral player

        //Step 3: Update effect string
        this -> setEffect("Blockade executed: " + target -> getID() + " now has " 
                          + std::to_string(target->getNumArmies()) +
                          " armies and belongs to Neutral player (" +
                          neutralPlayer->getPlayerName() + ").");

        cout << this->getEffect() << std::endl;
    
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

        //Make sure that the player, and the source / target territories, exist.
        if(source == nullptr || target == nullptr || issuer == nullptr) { return false; }

        //Make sure the player owns both the source and the target territory
        if(source -> getOwner() != issuer || target -> getOwner() != issuer) {

            return false;
        }

        // Check if enough armies to move (must leave at least 1 behind)
        if(numArmies <= 0 || source -> getNumArmies() <= 1) {

            return false;
        
        }

        //Check if the number of armys proposed to be moved is small enough to be valid 
        if(numArmies > source -> getNumArmies() - 1) { return false; }

        return true;

    }


    void Airlift::execute() {

        if(!this->validate()) {

            this -> setEffect("Airlift order invalid.");
            cout << this -> getEffect() << endl;
            return;
        
        }

        // --- Perform the move ---
        int armiesToMove = min(numArmies, source -> getNumArmies() - 1);


        source -> setNumArmies(source -> getNumArmies() - armiesToMove);
        target -> setNumArmies(target -> getNumArmies() + armiesToMove);

        // --- Update effect ---
        this -> setEffect("Airlift successful: " + issuer -> getPlayerName() + 
                          " moved " + to_string(armiesToMove) +
                          " armies from " + source -> getID() +
                          " to " + target -> getID() + ".");
        
        cout << this -> getEffect() << endl;

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

        //Make sure both the source and target players exist
        if(issuer == nullptr || targetPlayer == nullptr) { return false; }

        //Player can't negotiate with themselves
        if(issuer == targetPlayer) { return false; }

        return true;

    }

    void Negotiate::execute() {

        // Validate order
        if (!this->validate()) {
            this->setEffect("Negotiate order invalid.");
            std::cout << this->getEffect() << std::endl;
            return;
        }

        //Apply neutrality
        issuer -> addNeutralEnemy(targetPlayer -> getPlayerName());
        targetPlayer -> addNeutralEnemy(issuer -> getPlayerName());

        //Update effect
        this -> setEffect("Negotiate successful: " + issuer -> getPlayerName() +
                        " and " + targetPlayer -> getPlayerName() +
                        " cannot attack each other this turn.");

        std::cout << this->getEffect() << std::endl;

    }


    // ================= OrderList ================= //

    //-- Constructors, Destructor, Copy Constructor, Assignment Operator, Stream Insertion Operator --//

    OrderList::OrderList() {

        this -> orders = vector<Order*>(); //Initialize empty container

    }

    OrderList::~OrderList() {

        for(Order* o : orders) { delete o; } //Delete all orders in the list

    }

    OrderList::OrderList(const OrderList& other) {

        this -> orders = vector<Order*>();

        for(Order* o : other.orders) { //Deep copy using clone()

            orders.push_back(o -> clone());

        }

    }

    OrderList& OrderList::operator=(const OrderList& other) {

        if(this != &other) {

            for(Order* o : this -> orders) { delete o; } //Delete existing orders in the list
            orders.clear();

            //Rebuild container with deep copy
            this -> orders = vector<Order*>();

            for(Order* o : other.orders) {

                orders.push_back(o -> clone());

            }

        }

        return *this;

    }

    std::ostream& operator<<(std::ostream& os, const OrderList& ol) {

        os << "OrderList:" << std::endl;

        for (size_t i = 0; i < ol.orders.size(); i++) {//Iterate over all orders

            os << "  [" << i << "] " << *ol.orders[i] << std::endl;

        }

        return os;
    }

    //-- Accessors and Mutators --//

    //vector<Order*> OrderList::getOrders() const { return orders; }

    void OrderList::setOrders(const vector<Order*> newOrders) { 
    
        // Clean up existing orders
        for(Order* o : orders) { delete o; }

        // Allocate a new vector and clone each order
        this -> orders = vector<Order*>();

        for(Order* o : newOrders) {

            orders.push_back(o -> clone()); 

        }

    }


    //-- Class Methods --//

    void OrderList::addOrder(Order* o) {

        if(o != nullptr) { orders.push_back(o); }
    
    }

    void OrderList::removeOrder(int index) {

        if(index >= 0 && index < static_cast<int>(orders.size())) { //Check if the inputed index is valid

            delete orders[index]; //Free memory of the removed order
            orders.erase(orders.begin() + index);  //Shift all elements in the OrderList to the left by 1 index

        }

    }

    void OrderList::removeOrder(Order* orderPtr) {

        if(orderPtr == nullptr){ return; }

        for(size_t i = 0; i < orders.size(); i++){

            if(orders[i] == orderPtr) {
                delete orders[i];                      // free memory
                orders.erase(orders.begin() + i);      // erase slot
                return;
            
            }
        
        }
    
    }

    void OrderList::moveOrder(int oldPos, int newPos){

        //Check if the positions are valid
        bool geqZeroOldPos = oldPos >= 0;
        bool leqListSizeOldPos = oldPos < static_cast<int>(orders.size());
        bool geqZeroNewPos = newPos >= 0;
        bool leqListSizeNewPos = newPos < static_cast<int>(orders.size());

        if(geqZeroOldPos && leqListSizeOldPos && geqZeroNewPos && leqListSizeNewPos) { //Perform check

            Order* tempOrder = orders[oldPos]; //Take out the order
            orders.erase(orders.begin() + oldPos); //Remove from old position
            orders.insert(orders.begin() + newPos, tempOrder); //Insert at new position

        }

    }

    size_t OrderList::size() const {

        return orders.size();

    }

}