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

    string Order::getOrderTypeString() const {

        switch (orderType) {

            case(OrderType::Deploy): return "Deploy";
            case(OrderType::Advance): return "Advance";
            case(OrderType::Bomb): return "Bomb";
            case(OrderType::Blockade): return "Blockade";
            case(OrderType::Airlift): return "Airlift";
            case(OrderType::Negotiate): return "Negotiate";
            default: return "Unknown";

        }

    }


    void Order::setOrderType(OrderType t) { this -> orderType = t; }

    const string& Order::getEffect() const { return this -> effect; }

    void Order::setEffect(const string& e) { this -> effect = e; }

    //-- Interface Class Methods --//

    std::string Order::stringToLog() {
        return "[OrderLog] " + getOrderTypeString() + " | Effect: " + (effect.empty() ? "None" : effect);
    }



    /*------------------------------------------ SINGLE ORDER SUB-CLASSES ------------------------------------------------*/
    // ================= Deploy ================= //

    //-- Constructors, Destructor, Copy Constructor, Assignment Operator, Stream Insertion Operator --//

    Deploy::Deploy(Player* p, Territory* t, int armies) {

        this -> orderType = OrderType::Deploy;
        this -> issuer = p;
        this -> target = t;
        this -> numArmies = armies;

        //Safety checks to prevent invalid dereferencing
        if(p == nullptr && t == nullptr){

            this -> effect = "Invalid Deploy order: issuer and target are null.";
            return;

        } else if(p == nullptr){

            this -> effect = "Invalid Deploy order: issuer is null.";
            return;

        } else if(t == nullptr){

            this -> effect = "Player " + p -> getPlayerName() + " attempted to deploy to a null territory.";
            return;

        }

        //Construct the descriptive effect string safely
        this -> effect = "Player " + p -> getPlayerName() + " deploys " 
                        + to_string(armies) + " army/armies to territory " 
                        + t -> getID() + ".";

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
           << " with " << numArmies;

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

    pair<bool, string> Deploy::validate() const {

        ostringstream output;

        //Check if either the territory or the player are null pointers
        if(issuer == nullptr || target == nullptr){ 
        
            return {false, "[IssueOrder] This deploy order is invalid, at least one inputted pointer (issuer/target) is null."}; 
        
        } 

        // Territory must belong to the issuing player
        if(target -> getOwner() != issuer){


            output << "[IssueOrder] This deploy order is invalid, target territory "
                   << target -> getID() << " is owned not owned by the issuer, "
                   << issuer -> getPlayerName()
                   << ". It is owned by "
                   << (target -> getOwner() ? target -> getOwner() -> getPlayerName() : "an unknown player")
                   << ".";

            return {false, output.str()}; 

        }

        // Army count must be positive
        if(numArmies <= 0){ 

            output << "[IssueOrder] This deploy order is invalid, target territory "
                   << target -> getID() << " does not contain a sufficent number of armies ("
                   << to_string(target -> getNumArmies())
                   << ").";
            
            return {false, output.str()}; 

        }

        return {true, "[IssueOrder] This deploy order is valid"}; //Return true if all conditions are met

    }

    void Deploy::execute() {

        pair<bool, string> validateResults = validate();

        if(validateResults.first == false) {

            this -> setEffect(validateResults.second);
            notify(this);
            return;

        }

        // Add armies to target
        target->setNumArmies(target->getNumArmies() + numArmies);

        // --- Safeguard: ensure target is tracked by issuer ---
        if (!issuer->getOwnedTerritories().owns(target)) { issuer->addOwnedTerritories(target); }

        this -> setEffect("Deploy successful: placed " + to_string(numArmies) +
                        " armies on " + target->getID() + ".");

        notify(this);

    }


    // ================= Advance ================= //

    //-- Constructors, Destructor, Copy Constructor, Assignment Operator, Stream Insertion Operator --//

    Advance::Advance(Player* p, Territory* s, Territory* t, int armies) {

        this -> orderType = OrderType::Advance;
        this -> issuer = p;
        this -> source = s;
        this -> target = t;
        this -> numArmies = armies;

        //Safety checks to prevent invalid dereferencing
        if(s == nullptr && t == nullptr){

            this -> effect = "Player " + p -> getPlayerName() + " attempted to advance from and to null territories.";
            return;

        } else if(s == nullptr){

            this -> effect = "Player " + p -> getPlayerName() + " attempted to advance from a null source territory.";
            return;

        } else if(t == nullptr){

            this -> effect = "Player " + p -> getPlayerName() + " attempted to advance to a null target territory.";
            return;

        }

        //Construct effect string safely
        this -> effect = "Player " + p -> getPlayerName() + " intends to move " 
                        + to_string(armies) + " army/armies, from " 
                        + s -> getID() + ", to " + t -> getID() + ".";

    }

    //When order is deleted, it should NOT remove any other pointers, since they still exist
    Advance::~Advance() = default;

    Advance::Advance(const Advance& other) : Order(other) {

        this -> issuer = other.issuer;
        this -> source = other.source;
        this -> target = other.target;
        this -> numArmies = other.numArmies;
        this -> effect = other.effect;

    }

    Advance& Advance::operator=(const Advance& other) {

        if(this != &other) {

            Order::operator=(other);
            this -> issuer = other.issuer;
            this -> source = other.source;
            this -> target = other.target;
            this -> numArmies = other.numArmies;
            this -> effect = other.effect;
        
        }

        return *this;

    }

    void Advance::print(std::ostream& os) const {

        os << "Advance Order: "
           << (issuer ? issuer -> getPlayerName() : "Unknown Player") //Get player name
           << " moves " << numArmies << " armies " //Get number of armies
           << "from " << (source ? source -> getID() : "Unknown Source") //Get source territory
           << ", owned by: " << (source ? source -> getOwner() -> getPlayerName() : "Unknown Player")
           << " to " << (target ? target -> getID() : "Unknown Target") //Get target territory
           << ", owned by: " << (target ? target -> getOwner() -> getPlayerName() : "Unknown Player");

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

    pair<bool, string> Advance::validate() const {

        ostringstream output; // Collect validation details

        //Check if either of the territories, or the player, are null pointers
        if(issuer == nullptr || source == nullptr || target == nullptr) { 
            
            return { false, "[IssueOrder] This advance order is invalid: one or more inputted pointers (issuer/source/target) are null." }; 
        
        }

        //Player must own the source territory
        if(source -> getOwner() != issuer){ 

            output << "[IssueOrder] This advance order is invalid: source territory "
                   << source -> getID() << " is not owned by the issuing player "
                   << issuer -> getPlayerName() << ". It is owned by "
                   << (source -> getOwner() ? source -> getOwner() -> getPlayerName() : "an unknown player")
                   << ".";

            return { false, output.str() }; 

        }

        //Player must have enough armies, or the number of armies they use must be geq 0
        if(numArmies <= 0 || source -> getNumArmies() < numArmies) { 

            output << "[IssueOrder] This advance order is invalid: attempted to move "
                   << numArmies << " armies, but source territory "
                   << source -> getID() << " only has "
                   << source -> getNumArmies() << " armies available.";

            return { false, output.str() }; 

        }

        //Any advance order requires the target to be adjacent to the source
        vector<Territory*> neighbors = source -> getNeighbors();

        //Check if the target territory is a neighbour of the source territory
        bool isAdjacent = (find(neighbors.begin(), neighbors.end(), target) != neighbors.end());
        if(!isAdjacent) { 

            output << "[IssueOrder] This advance order is invalid: target territory "
                   << target -> getID() << " is not adjacent to source territory "
                   << source -> getID() << ".";

            return { false, output.str() }; 

        }

        //Check if the attacker has a truce with the defender
        Player* defender = target -> getOwner();
        
        if(defender != nullptr) {

            const vector<Player*>& neutrals = issuer -> getNeutralEnemies(); //List of players under truce

            //If the defender is in the neutral list, advancing is invalid
            if(find(neutrals.begin(), neutrals.end(), defender) != neutrals.end()) { 

                output << "[IssueOrder] This advance order is invalid: the issuer ("
                       << issuer -> getPlayerName() << ") currently has a truce with "
                       << defender -> getPlayerName() << ", and cannot attack them.";

                return { false, output.str() }; 
            
            }
        
        }

        return { true, "[IssueOrder] This advance order is valid." };

    }


    void Advance::execute() {

        // --- 1. Validate ownership and adjacency ---
        pair<bool, string> validateResults = validate();
        if(!validateResults.first) {

            this -> setEffect(validateResults.second);
            notify(this);
            return;

        }

        // --- 2. Check if source belongs to issuer ---
        if(source -> getOwner() != issuer) {

            this -> setEffect("Advance failed: source territory not owned by issuer.");
            return;

        }

        // --- 3. Check adjacency ---
        const vector<Territory*>& neighbors = source -> getNeighbors();

        //Check if the target is in the neighbour vector
        if(find(neighbors.begin(), neighbors.end(), target) == neighbors.end()){

            this -> setEffect("Advance failed: target territory is not adjacent.");
            return;

        }

        // --- 4. If target is also owned by issuer, reinforce ---
        if (target->getOwner() == issuer) {

            if (source->getNumArmies() <= 1) { //1 Army MUST remain on the source 

                this -> setEffect("Advance failed: Not enough armies to advance with.");
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

            return;
        
        }


        // --- 5. Perform battle logic
        if(source -> getNumArmies() <= 1) { //1 Army MUST remain on the source 

            this -> setEffect("Advance failed: Not enough armies to attack with.");
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

        if(defendersRemaining <= 0) {

            if(attackersRemaining > 1) { // Successful conquest

                //Transfer ownership
                issuer -> addOwnedTerritories(target);
                target -> setNumArmies(attackersRemaining);

                issuer -> setGenerateCardThisTurn(true); //Attacker earns a card at turn end

                this -> setEffect("Advance battle successful: " + issuer->getPlayerName() +
                                  " defeated " + oldOwnerName +
                                  ", and conquered " + target->getID() + " with " +
                                  to_string(attackersRemaining) + " surviving armies.");

            } else { //Edge case: attacker “wins” but can’t move in

                defendersRemaining = 1;
                target -> setNumArmies(defendersRemaining);

                this->setEffect("Advance battle inconclusive: " +
                                issuer->getPlayerName() + " tried to beat " + oldOwnerName +
                                ", but didn't have enough armies to move in. The defender clings onto " + target->getID() +
                                " with 1 army, after a desperate defense.");

            }

        } else { // Defender Wins

            target -> setNumArmies(defendersRemaining);

            this->setEffect("Advance battle failed: defender " +
                            oldOwnerName + " beat " + issuer -> getPlayerName() + ", and holds onto "
                            + target -> getID() + " with " +
                            to_string(defendersRemaining) + " armies remaining.");

        }

        // If a neutral player was attacked, change their strategy to aggressive.
        if (oldOwner->getStrategyType() == PlayerStrategyType::NEUTRAL)
            oldOwner->setStrategyType(PlayerStrategyType::AGGRESSIVE);

        notify(this);

    }

    // ================= Bomb ================= //

    //-- Constructors, Destructor, Copy Constructor, Assignment Operator, Stream Insertion Operator --//

    Bomb::Bomb(Player* p, Territory* t) {

        this -> orderType = OrderType::Bomb;
        this -> issuer = p;
        this -> target = t;

        //Safety checks to avoid invalid memory access
        if(t == nullptr){

            this -> effect = "Player " + p -> getPlayerName() + " attempted to bomb a null target.";
            return;

        }

        Player* targetOwner = t -> getOwner();

        if(targetOwner == nullptr){

            this -> effect = "Player " + p -> getPlayerName() + " attempted to bomb an unowned territory (ID: " 
                        + t -> getID() + ").";
            return;

        }

        //Build the descriptive effect string
        this -> effect = "Player " + p -> getPlayerName() + " intends to bomb "
            + targetOwner -> getPlayerName() + " on territory "
            + t -> getID() + ". This territory contains "
            + to_string(t -> getNumArmies()) + " armies.";

    }


    //When order is deleted, it should NOT remove any other pointers, since they still exist
    Bomb::~Bomb() = default; 

    Bomb::Bomb(const Bomb& other) : Order(other) {

        this -> issuer = other.issuer;
        this -> target = other.target;
        this -> effect = other.effect;

    }

    Bomb& Bomb::operator=(const Bomb& other) {

        if(this != &other) {

            Order::operator=(other);
            this -> issuer = other.issuer;
            this -> target = other.target;
            this -> effect = other.effect;
            
        }

        return *this;

    }

    void Bomb::print(std::ostream& os) const {

        os << "Bomb Order: "
           << (issuer ? issuer -> getPlayerName() : "Unknown Player") //Get player name
           << " bombs " << (target ? target -> getID() : "Unknown Territory") //Get target territory name
           << ", owned by " << (target && target -> getOwner() ? target -> getOwner() -> getPlayerName() : "Unknown Player") //Get target territory owner
           << ". This territory has " << (target ? to_string(target -> getNumArmies()) : "Unknown") << " armies on it"; //Get number of armies

    }

    //-- Accessors and Mutators --//

    Player* Bomb::getIssuer() const { return issuer; }
    void Bomb::setIssuer(Player* p) { issuer = p; }

    Territory* Bomb::getTarget() const { return target; }
    void Bomb::setTarget(Territory* t) { target = t; }

    //-- Overridden Class Methods --//

    Order* Bomb::clone() const { return new Bomb(*this); }

    pair<bool, string> Bomb::validate() const {

        ostringstream output; // Collect validation details

        //Make sure player and territory target aren't nullpointers 
        if(issuer == nullptr || target == nullptr) { 
            
            return { false, "[IssueOrder] This bomb order is invalid: one or more inputted pointers (issuer/target) is a null pointer." }; 
        
        }

        //Check if the target territory has an owner
        Player* defender = target -> getOwner();
        if(defender == nullptr) {

            output << "[IssueOrder] This bomb order is invalid: target territory "
                   << target -> getID() << " has no owner.";

            return{ false, output.str() };

        }

        //Check if the target territory belongs to the issuing player
        if(defender == issuer) {

            output << "[IssueOrder] This bomb order is invalid: the issuer ("
                   << issuer -> getPlayerName() << ") cannot bomb their own territory ("
                   << target -> getID() << ").";
            return { false, output.str() };

        }

        //Check if the attacker has a truce with the defender
        const vector<Player*>& neutrals = issuer -> getNeutralEnemies(); //List of players under truce

        if(find(neutrals.begin(), neutrals.end(), defender) != neutrals.end()) { 

            output << "[IssueOrder] This bomb order is invalid: the issuer ("
                   << issuer -> getPlayerName() << ") currently has a truce with "
                   << defender -> getPlayerName()
                   << ", and cannot target their territories.";
            return { false, output.str() }; 

        }

        //Use the player's existing bomb candidate logic for validation
        vector<Territory*> bombCandidates = issuer -> getBombCandidates();

        //Target must exist in the list of valid bombable territories
        bool isValidTarget = (find(bombCandidates.begin(), bombCandidates.end(), target) != bombCandidates.end());
        if(!isValidTarget) {

            output << "[IssueOrder] This bomb order is invalid: target territory "
                   << target -> getID()
                   << " is not a valid bombing target (not adjacent to owned territory).";
            return { false, output.str() }; 

        }

        return { true, "[IssueOrder] This bomb order is valid." };

    }


    void Bomb::execute() {

        pair<bool, string> validateResults = validate();
        if(!validateResults.first) {

            this -> setEffect(validateResults.second);
            notify(this);
            return;

        }

        int currentArmies = target -> getNumArmies();
        int armiesToRemove = currentArmies / 2; // Remove half, rounded down
        
        target -> setNumArmies(currentArmies - armiesToRemove);

        this->setEffect("Bomb order executed. Player " + target -> getOwner() -> getPlayerName() + ", at territory " 
                        + target -> getID() + ", lost " + to_string(armiesToRemove) + " armies.");

        notify(this);
        
    }

    // ================= Blockade ================= //

    //-- Constructors, Destructor, Copy Constructor, Assignment Operator, Stream Insertion Operator --//

    Blockade::Blockade(Player* p, Territory* t, Player* neutral) {

        this -> orderType = OrderType::Blockade;
        this -> issuer = p;
        this -> target = t;
        this -> neutralPlayer = neutral;

        //Safety checks to prevent invalid dereferencing
        if(p == nullptr && t == nullptr && neutral == nullptr){

            this -> effect = "Invalid Blockade order: all parameters are null.";
            return;

        } else if(p == nullptr){

            this -> effect = "Invalid Blockade order: issuer is null.";
            return;

        } else if(t == nullptr){

            this -> effect = "Player " + p -> getPlayerName() + " attempted to blockade a null territory.";
            return;

        } else if(neutral == nullptr){

            this -> effect = "Player " + p -> getPlayerName() + " attempted to blockade territory "
                            + t -> getID() + ", but the neutral player is null.";
            return;

        }

        //Construct the descriptive effect string safely
        this -> effect = "Player " + p -> getPlayerName() + " intends to blockade territory "
                        + t -> getID() + ". Neutral player " + neutral -> getPlayerName()
                        + " will receive the territory, and will have "
                        + to_string(3 * (t -> getNumArmies()))
                        + " armies to defend with.";

    }


    //When order is deleted, it should NOT remove any other pointers, since they still exist
    Blockade::~Blockade() = default;

    Blockade::Blockade(const Blockade& other) : Order(other) {

        this -> issuer = other.issuer;
        this -> target = other.target;
        this -> neutralPlayer = other.neutralPlayer;
        this -> effect = other.effect;

    }

    Blockade& Blockade::operator=(const Blockade& other) {

        if(this != &other) {

            Order::operator=(other);
            this -> issuer = other.issuer;
            this -> target = other.target;
            this -> neutralPlayer = other.neutralPlayer;
            this -> effect = other.effect;

        }

        return *this;

    }

    void Blockade::print(std::ostream& os) const {

        os << "Blockade Order: "
           << (issuer ? issuer -> getPlayerName() : "Unknown Player") //Get player name
           << " tries to blockade " << (target ? target -> getID() : "Unknown Territory") //Get target territory
           << ". They have " << issuer -> getOwnedTerritories().size() << " territory/territories"
           << ". Territory is attempted to be transferred to a neutral player, " 
           << (neutralPlayer ? neutralPlayer -> getPlayerName() : "UNKNOWN NAME"); //Get neutral player

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

    pair<bool, string> Blockade::validate() const {

        ostringstream output; // Collect validation details

        //Check if the target territory, the issuing player, or the neutral player are null pointers
        if(issuer == nullptr || target == nullptr || neutralPlayer == nullptr) { 
            
            return { false, "[IssueOrder] This blockade order is invalid: one or more inputted pointers (issuer/target/neutral) is null." }; 
        
        }

        //Target must belong to the issuing player
        if(target -> getOwner() != issuer) { 

            output << "[IssueOrder] This blockade order is invalid: target territory "
                   << target -> getID() << " is not owned by the issuing player "
                   << issuer -> getPlayerName() << ". It is currently owned by "
                   << (target -> getOwner() ? target -> getOwner() -> getPlayerName() : "an unknown player")
                   << ".";

            return { false, output.str() }; 

        }

        //Player must own more than 1 territory. They would make themselves lose otherwise
        size_t numTerritories = issuer -> getOwnedTerritories().getTerritories().size();
        if(numTerritories <= 1) { 

            output << "[IssueOrder] This blockade order is invalid: the issuing player ("
                   << issuer -> getPlayerName()
                   << ") only owns " << numTerritories
                   << " territory. Performing a blockade would result in having no owned territories left.";

            return { false, output.str() }; 

        }

        //TODO: When adding player pattern, check if netral has neutral behaviour implemented

        return { true, "[IssueOrder] This blockade order is valid." };

    }


    void Blockade::execute() {

        pair<bool, string> validateResults = validate();
        if(!validateResults.first) {

            this -> setEffect(validateResults.second);
            notify(this);
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

        notify(this);
    
        notify(this); // Notify observers about the execution of this order
    }



    // ================= Airlift ================= //

    //-- Constructors, Destructor, Copy Constructor, Assignment Operator, Stream Insertion Operator --//

    Airlift::Airlift(Player* p, Territory* s, Territory* t, int armies) {

        this -> orderType = OrderType::Airlift;
        this -> issuer = p;
        this -> source = s;
        this -> target = t;
        this -> numArmies = armies;

        //Safety checks to prevent invalid dereferencing
        if(p == nullptr && s == nullptr && t == nullptr){

            this -> effect = "Invalid Airlift order: all parameters are null.";
            return;

        } else if(p == nullptr){

            this -> effect = "Invalid Airlift order: issuer is null.";
            return;

        } else if(s == nullptr && t == nullptr){

            this -> effect = "Player " + p -> getPlayerName() + " attempted to airlift between null territories.";
            return;

        } else if(s == nullptr){

            this -> effect = "Player " + p -> getPlayerName() + " attempted to airlift from a null source territory.";
            return;

        } else if(t == nullptr){

            this -> effect = "Player " + p -> getPlayerName() + " attempted to airlift to a null target territory.";
            return;

        }

        //Construct the descriptive effect string safely
        this -> effect = "Player " + p -> getPlayerName() + " intends to transfer "
                        + to_string(armies) + " army/armies, from territory "
                        + s -> getID() + ", to territory " + t -> getID() + ".";

    }

    //When order is deleted, it should NOT remove any other pointers, since they still exist
    Airlift::~Airlift() = default;

    Airlift::Airlift(const Airlift& other) : Order(other) {

        this -> issuer = other.issuer;
        this -> source = other.source;
        this -> target = other.target;
        this -> numArmies = other.numArmies;
        this -> effect = other.effect;

    }

    Airlift& Airlift::operator=(const Airlift& other) {

        if(this != &other) {

            Order::operator=(other);
            this -> issuer = other.issuer;
            this -> source = other.source;
            this -> target = other.target;
            this -> numArmies = other.numArmies;
            this -> effect = other.effect;

        }

        return *this;

    }

    void Airlift::print(std::ostream& os) const {

        os << "Airlift Order: "
           << (issuer ? issuer -> getPlayerName() : "Unknown Player") //Get player name
           << " airlifts " << numArmies << " armies " //Get number of armies
           << "from " << (source ? source -> getID() : "Unknown Source") //Get source territory
           << " to " << (target ? target -> getID() : "Unknown Target"); //Get target territory

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

    pair<bool, string> Airlift::validate() const {

        ostringstream output; // Collect validation details

        //Make sure that the player, and the source / target territories, exist.

        if(issuer == nullptr) {

            return { false, "This airlift order is invalid, as the Issuer territory pointer is null\n" }; 

        }

        if(source == nullptr) {

            return { false, "This airlift order is invalid, as the Source territory pointer is null\n" }; 

        }

        if(target == nullptr) {

            return { false, "This airlift order is invalid, as the Target territory pointer is null\n" }; 

        }

        if(source == nullptr && target == nullptr) {

            return { false, "This airlift order is invalid, as both the Source and Target territory pointers are null\n" }; 

        }

        //Make sure the player owns both the source and the target territory
        if(source -> getOwner() != issuer || target -> getOwner() != issuer) {

            output << "[IssueOrder] This airlift order is invalid: both source and target territories must be owned by the issuing player "
                   << issuer -> getPlayerName() << ". Source territory '"
                   << source -> getID() << "' is owned by "
                   << (source -> getOwner() ? source -> getOwner() -> getPlayerName() : "unknown")
                   << ", and target territory '" << target -> getID() << "' is owned by "
                   << (target -> getOwner() ? target -> getOwner() -> getPlayerName() : "unknown") << ".";

            return { false, output.str() }; 

        }

        // Check if enough armies to move (must leave at least 1 behind)
        if(numArmies <= 0 || source -> getNumArmies() <= 1) {

            output << "[IssueOrder] This airlift order is invalid: source territory "
                   << source -> getID() << " does not have enough armies to perform an airlift. "
                   << "Current armies: " << source -> getNumArmies() 
                   << ", armies requested: " << numArmies << ".";

            return { false, output.str() }; 
        
        }

        //Check if the number of armies proposed to be moved is small enough to be valid 
        if(numArmies > source -> getNumArmies() - 1) { 

            output << "[IssueOrder] This airlift order is invalid: attempted to move "
                   << numArmies << " armies, but only "
                   << (source -> getNumArmies() - 1)
                   << " can be moved (must leave at least 1 behind).";

            return { false, output.str() }; 

        }

        return { true, "[IssueOrder] This airlift order is valid." };

    }

    void Airlift::execute() {

        pair<bool, string> validateResults = validate();
        if(!validateResults.first) {

            this -> setEffect(validateResults.second);
            notify(this);
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

        notify(this);
        
        notify(this); // Notify observers about the execution of this order
    }



    // ================= Negotiate ================= //

    //-- Constructors, Destructor, Copy Constructor, Assignment Operator, Stream Insertion Operator --//

    Negotiate::Negotiate(Player* p, Player* other) {

        this -> orderType = OrderType::Negotiate;
        this -> issuer = p;
        this -> targetPlayer = other;

        //Safety checks to prevent invalid dereferencing
        if(p == nullptr && other == nullptr){

            this -> effect = "Invalid Negotiate order: both players are null.";
            return;

        } else if(p == nullptr){

            this -> effect = "Invalid Negotiate order: issuer is null.";
            return;

        } else if(other == nullptr){

            this -> effect = "Player " + p -> getPlayerName() + " attempted to negotiate with a null player.";
            return;

        }

        //Construct the descriptive effect string safely
        this -> effect = "Player " + p -> getPlayerName() 
                        + " intends to declare a truce with player "
                        + other -> getPlayerName() + ".";

    }


    //When order is deleted, it should NOT remove any other pointers, since they still exist
    Negotiate::~Negotiate() = default;

    Negotiate::Negotiate(const Negotiate& other) : Order(other) {

        this -> issuer = other.issuer;
        this -> targetPlayer = other.targetPlayer;
        this -> effect = other.effect;

    }

    Negotiate& Negotiate::operator=(const Negotiate& other) {

        if(this != &other) {

            Order::operator=(other);
            this -> issuer = other.issuer;
            this -> targetPlayer = other.targetPlayer;
            this -> effect = other.effect;

        }

        return *this;

    }

    void Negotiate::print(std::ostream& os) const {

        os << "Negotiate Order: "
           << (issuer ? issuer -> getPlayerName() : "Unknown Player") //Get issuing player
           << " negotiates peace with " 
           << (targetPlayer ? targetPlayer -> getPlayerName() : "Unknown Player"); //Get target player

    }

    //-- Accessors and Mutators --//

    Player* Negotiate::getIssuer() const { return issuer; }
    void Negotiate::setIssuer(Player* p) { issuer = p; }

    Player* Negotiate::getTargetPlayer() const { return targetPlayer; }
    void Negotiate::setTargetPlayer(Player* other) { targetPlayer = other; }

    //-- Overridden Class Methods --//

    Order* Negotiate::clone() const { return new Negotiate(*this); }

    pair<bool, string> Negotiate::validate() const {

        ostringstream output; // Collect validation details

        //Make sure both the source and target players exist
        if(issuer == nullptr || targetPlayer == nullptr) { 
            
            return { false, "[IssueOrder] This negotiate order is invalid: one of the inputted pointers (issuer/targetPlayer) is null." }; 
        
        }

        //Player can't negotiate with themselves
        if(issuer == targetPlayer) { 
            
            output << "[IssueOrder] This negotiate order is invalid: the issuer ("
                   << issuer -> getPlayerName()
                   << ") cannot negotiate with themselves.";

            return { false, output.str() }; 
        
        }

        //Check if a truce has already been declared between the issuer and target
        const vector<Player*>& neutrals = issuer -> getNeutralEnemies(); //List of players the issuer has a truce with

        if(find(neutrals.begin(), neutrals.end(), targetPlayer) != neutrals.end()) { 
            
            output << "[IssueOrder] This negotiate order is invalid: a truce already exists between "
                   << issuer -> getPlayerName()
                   << " and " << targetPlayer -> getPlayerName()
                   << ".";

            return { false, output.str() }; 
            
        }

        return { true, "[IssueOrder] This negotiate order is valid." };

    }

    void Negotiate::execute() {

        // Validate order
        pair<bool, string> validateResults = validate();
        if(!validateResults.first) {

            this -> setEffect(validateResults.second);
            notify(this);
            return;

        }

        //Apply neutrality
        issuer -> addNeutralEnemy(targetPlayer);
        targetPlayer -> addNeutralEnemy(issuer);

        //Update effect
        this -> setEffect("Negotiate successful: " + issuer -> getPlayerName() +
                        " and " + targetPlayer -> getPlayerName() +
                        " cannot attack each other this turn.");

        notify(this);

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

    vector<Order*>& OrderList::getOrders() { return orders; }
    
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

        if(o != nullptr) { 
            
            orders.push_back(o); 
            notify(this);

        }
    
    }

    void OrderList::removeOrder(int index) {

        if(index >= 0 && index < static_cast<int>(orders.size())) { //Check if the inputed index is valid

            delete orders[index]; //Free memory of the removed order
            orders.erase(orders.begin() + index);  //Shift all elements in the OrderList to the left by 1 index
            notify(this);

        }

    }

    void OrderList::removeOrder(Order* orderPtr) {

        if(orderPtr == nullptr){ return; }

        for(size_t i = 0; i < orders.size(); i++){

            if(orders[i] == orderPtr) {
                delete orders[i];                      // free memory
                orders.erase(orders.begin() + i);      // erase slot
                notify(this);
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
            notify(this);

        }

    }

    Order* OrderList::peek() const {
        
        if(!orders.empty()){ return orders.front(); }
        return nullptr;
        
    }

    Order* OrderList::back() const {
        
        if(!orders.empty()) { return orders.back(); }
        return nullptr; 
        
    }

    void OrderList::replaceOrder(int index, Order* newOrder) {
        
        notify(this);

        //Verify both newOrder pointer is valid
        if(newOrder == nullptr){ return; }

        //Check if the list is empty
        if(orders.empty()){ return; }

        //Check if index is valid
        if(index < 0 || index >= static_cast<int>(orders.size())) { return; }

        //Delete the old order safely
        Order* oldOrder = orders[index];
        if(oldOrder != nullptr) {

            delete oldOrder;
            oldOrder = nullptr;
        
        }

        //Replace in place (ordinality preserved)
        orders[index] = newOrder;

    }

    void OrderList::replaceOrder(Order* oldOrder, Order* newOrder) {
        
        notify(this);

        //Verify both Order pointers are valid
        if(oldOrder == nullptr || newOrder == nullptr){ return; }

        //Check if the list is empty
        if(orders.empty()){ return; }

        //Find position of old order
        auto it = std::find(orders.begin(), orders.end(), oldOrder);
        
        //If not found, return
        if(it == orders.end()){ return; }

        // --- Delete the old order safely ---
        delete *it;

        // --- Replace in place (ordinality preserved) ---
        *it = newOrder;
    
    }

    size_t OrderList::size() const { return orders.size(); }

    std::string OrderList::stringToLog(){
        if(orders.empty()) {
            return "[OrderList] No orders.";
        }
        Order* order = orders.back();
        return "[OrderList] Order Issued: " + order->getOrderTypeString();
    }

}