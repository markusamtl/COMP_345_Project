#pragma once

#include <vector>
#include <string>
#include <iostream>
#include <chrono>

#include "../Player/Player.h"

namespace WarzonePlayer { class Player; } //Forward declaration
using namespace std;

namespace WarzoneOrder {

    using WarzoneMap::Territory;
    using WarzoneMap::Map;
    using WarzonePlayer::Player;

    /*------------------------------------------TIME UTILITY CLASS--------------------------------------------------*/

    /**
     * @class TimeUtil
     * @brief Provides utility methods for retrieving system time with high resolution.
     * 
     * This class offers methods to fetch the current system time in seconds,
     * milliseconds, or nanoseconds since epoch.
     * 
     * USE THIS CLASS INSTEAD OF rand(), AS IT ENSURES THERE IS NO NEED TO SEED.
     * 
     * At millisecond and nanosecond timeframes, system processes, CPU scheduling,
     * and hardware noise make it functionally impossible to predict the exact
     * values returned. This provides time-based entropy that generates 
     * effectively unpredictable numbers, which is sufficient for gameplay randomness.
     */
    class TimeUtil {

        public:

            /**
             * @brief Get the current system time in seconds (with fractional precision).
             * @return Current system time in seconds as a double.
             */
            static double getSystemTimeSeconds();

            /**
             * @brief Get the current system time in milliseconds since epoch.
             * @return Current system time in milliseconds as a long long.
             */
            static long long getSystemTimeMillis();

            /**
             * @brief Get the current system time in nanoseconds since epoch.
             * @return Current system time in nanoseconds as a long long.
             */
            static long long getSystemTimeNano();
    };

    /**
     * @brief List of enums that helps to differentiate order types.
     * 
     * Each order type corresponds to a specific action in the Warzone game:
     * - Deploy: Place reinforcement armies from the player's pool onto one of their territories.
     * - Advance: Move armies from one territory to another, either to defend or to attack an enemy.
     * - Bomb: Destroy half of the armies on an opponent's territory (requires Bomb card).
     * - Blockade: Double the armies on one of the player’s territories and transfer ownership to Neutral (requires Blockade card).
     * - Airlift: Move armies between any two territories owned by the player, regardless of adjacency (requires Airlift card).
     * - Negotiate: Prevent attacks between two players for the remainder of the turn (requires Diplomacy card).
     */
    enum class OrderType {

        Deploy, Advance, Bomb, Blockade, Airlift, Negotiate 
    
    };

    /*------------------------------------------ORDER SUPER-CLASS------------------------------------------------*/
    /**
     * @class Order
     * @brief Abstract base class for all Warzone orders.
     * 
     * The Order class defines the interface that all specific Warzone orders must follow.
     * Each subclass of Order (Deploy, Advance, Bomb, Blockade, Airlift, Negotiate)
     * implements its own behavior for validation and execution according to the game rules.
     * 
     * @see Deploy
     * @see Advance
     * @see Bomb
     * @see Blockade
     * @see Airlift
     * @see Negotiate
     */
    class Order {

        protected:

            //-- Order Attributes --//

            OrderType orderType; //Not a pointer, since it's an enumurator class
            string effect;

        public:

        //-- Constructors, Destructor, Copy Constructor, Assignment Operator, Stream Insertion Operator --//

        /**
         * @brief Default constructor. Initializes an Order with no specific type or effect.
         */
        Order();

        /**
         * @brief Parameterized constructor.
         * 
         * Initializes an Order with a given OrderType.
         * @param t The type of the order (Deploy, Advance, Bomb, etc).
         */
        Order(OrderType t);

        /**
         * @brief Parameterized constructor.
         * 
         * Initializes an Order with a given OrderType.
         * @param t The type of the order (Deploy, Advance, Bomb, etc).
         * @param effect Output for what the order does.
         */
        Order(OrderType t, string effect);

        /**
         * @brief Virtual destructor.
         * 
         * Declared virtual to ensure proper cleanup of derived classes
         * when deleting through an Order* pointer.
         */
        virtual ~Order();

        /**
         * @brief Copy constructor.
         * 
         * Creates a deep copy of another Order, including its type and effect.
         * @param other The Order to copy from.
         */
        Order(const Order& other);

        /**
         * @brief Assignment operator.
         * 
         * Assigns the values of one Order to another, handling type and effect.
         * @param other The Order to copy from.
         * @return Reference to this Order after assignment.
         */
        Order& operator=(const Order& other);

        /**
         * @brief Outputs a textual representation of the order to a stream.
         * This method is intended to be overridden by each concrete subclass
         * (e.g., Deploy, Advance, Bomb, etc.) to provide customized output
         * that reflects the specific order's details.
         * @param os The output stream where the order information will be written.
         */
        virtual void print(std::ostream& os) const;

        /**
         * @brief Stream insertion operator.
         * Outputs a textual representation of the Order to a given output stream.
         * @param os The output stream.
         * @param order The Order to output.
         * @return Reference to the output stream.
         */
        friend std::ostream& operator<<(std::ostream& os, const Order& order);

        //-- Accessors and Mutators --//

        /**
         * @brief Get the type of this order.
         * @return The OrderType enum value.
         */
        OrderType getOrderType() const;

        /**
         * @brief Set the type of this order.
         * @param t The new OrderType value to assign.
         */
        void setOrderType(OrderType t);

        /**
         * @brief Get the effect string of this order.
         * @return The textual effect produced after execution.
         */
        const string& getEffect() const;

        /**
         * @brief Set the effect string of this order.
         * @param e The new effect string to assign.
         */
        void setEffect(const string& e);


        //-- Interface Class Methods --//

        /**
         * @brief Clone method (pure virtual).
         * Polymorphic copy function that creates a new Order of the same
         * dynamic type. Must be implemented in each subclass.
         * @return A pointer to a newly allocated copy of the Order.
         * 
         * @see Deploy::clone()
         * @see Advance::clone()
         * @see Bomb::clone()
         * @see Blockade::clone()
         * @see Airlift::clone()
         * @see Negotiate::clone()
         */
        virtual Order* clone() const = 0;

        /**
         * @brief Validate method (pure virtual).
         * 
         * Determines whether the Order is valid according to the game rules.
         * Must be implemented in each subclass.
         * @return True if the Order is valid, false otherwise.
         * 
         * @see Deploy::validate()
         * @see Advance::validate()
         * @see Bomb::validate()
         * @see Blockade::validate()
         * @see Airlift::validate()
         * @see Negotiate::validate()
         */
        virtual bool validate() const = 0;

        /**
         * @brief Execute method (pure virtual).
         * 
         * Executes the Order’s action if it is valid, updating the game state.
         * Must be implemented in each subclass.
         * 
         * @see Deploy::execute()
         * @see Advance::execute()
         * @see Bomb::execute()
         * @see Blockade::execute()
         * @see Airlift::execute()
         * @see Negotiate::execute()
         */
        virtual void execute() = 0;

    };

    /*------------------------------------------ SINGLE ORDER SUB-CLASSES------------------------------------------------*/
    /**
     * @class Deploy
     * @brief Represents a Deploy order.
     * 
     * A Deploy order places armies from the player’s reinforcement pool
     * onto one of their owned territories.
     */
    class Deploy : public Order {
        
        private:

            //Deploy Attributes
            Player* issuer;       
            Territory* target;                
            int numArmies;

        public:

            //-- Constructors, Destructor, Copy Constructor, Assignment Operator, Stream Insertion Operator --//

            /**
             * @brief Parameterized constructor.
             * @param p The player issuing the order.
             * @param t The target territory.
             * @param armies The number of armies to deploy.
             */
            Deploy(Player* p, Territory* t, int armies);

            /**
             * @brief Destructor.
             */
            ~Deploy() override;

            /**
             * @brief Copy constructor.
             * @param other The Deploy order to copy from.
             */
            Deploy(const Deploy& other);

            /**
             * @brief Assignment operator.
             * @param other The Deploy order to assign from.
             * @return Reference to this Deploy order.
             */
            Deploy& operator=(const Deploy& other);

            /**
             * @brief Print details specific to a Deploy order.
             * @param os The output stream.
             */
            void print(std::ostream& os) const override;

            // ---- Accessors and Mutators ---- //

            /**
             * @brief Get the issuing player of this order.
             * @return Pointer to the Player who issued the order.
             */
            Player* getIssuer() const;

            /**
             * @brief Set the issuing player of this order.
             * @param p Pointer to the Player issuing the order.
             */
            void setIssuer(Player* p);

            /**
             * @brief Get the target territory of this order.
             * @return Pointer to the target Territory.
             */
            Territory* getTarget() const;

            /**
             * @brief Set the target territory of this order.
             * @param t Pointer to the new target Territory.
             */
            void setTarget(Territory* t);

            /**
             * @brief Get the number of armies to deploy.
             * @return The number of armies as an integer.
             */
            int getNumArmies() const;

            /**
             * @brief Set the number of armies to deploy.
             * @param armies The new number of armies.
             */
            void setNumArmies(int armies);

            //--- Overridden Class Methods ---//

            /**
             * @brief Polymorphic copy constructor.
             * @return A pointer to a new Deploy object.
             */
            Order* clone() const override;

            /**
             * @brief Validate the order.
             * @return True if the target belongs to the player, false otherwise.
             */
            bool validate() const override;

            /**
             * @brief Execute the order.
             * If valid, adds armies to the target territory.
             */
            void execute() override;

    };


    /**
     * @class Advance
     * @brief Represents an Advance order.
     * 
     * An Advance order moves armies from one territory to another,
     * either reinforcing friendly territory or attacking an enemy.
     */
    class Advance : public Order {
        
        private:

            // Advance Attributes
            Player* issuer;
            Territory* source;
            Territory* target;
            int numArmies;

        public:

            //-- Constructors, Destructor, Copy Constructor, Assignment Operator, Stream Insertion Operator --//

            /**
             * @brief Parameterized constructor.
             * @param p The player issuing the order.
             * @param s The source territory from which armies are moved.
             * @param t The target territory to which armies are moved.
             * @param armies The number of armies to advance.
             */
            Advance(Player* p, Territory* s, Territory* t, int armies);

            /**
             * @brief Destructor.
             */
            ~Advance() override;

            /**
             * @brief Copy constructor.
             * @param other The Advance order to copy from.
             */
            Advance(const Advance& other);

            /**
             * @brief Assignment operator.
             * @param other The Advance order to assign from.
             * @return Reference to this Advance order.
             */
            Advance& operator=(const Advance& other);

            /**
             * @brief Print details specific to an Advance order.
             * @param os The output stream.
             */
            void print(std::ostream& os) const override;

            // ---- Accessors and Mutators ---- //

            /**
             * @brief Get the issuing player of this order.
             * @return Pointer to the Player who issued the order.
             */
            Player* getIssuer() const;

            /**
             * @brief Set the issuing player of this order.
             * @param p Pointer to the Player issuing the order.
             */
            void setIssuer(Player* p);

            /**
             * @brief Get the source territory of this order.
             * @return Pointer to the source Territory.
             */
            Territory* getSource() const;

            /**
             * @brief Set the source territory of this order.
             * @param s Pointer to the new source Territory.
             */
            void setSource(Territory* s);

            /**
             * @brief Get the target territory of this order.
             * @return Pointer to the target Territory.
             */
            Territory* getTarget() const;

            /**
             * @brief Set the target territory of this order.
             * @param t Pointer to the new target Territory.
             */
            void setTarget(Territory* t);

            /**
             * @brief Get the number of armies to advance.
             * @return The number of armies as an integer.
             */
            int getNumArmies() const;

            /**
             * @brief Set the number of armies to advance.
             * @param armies The new number of armies.
             */
            void setNumArmies(int armies);

            //--- Overridden Class Methods ---//

            /**
             * @brief Polymorphic copy constructor.
             * @return A pointer to a new Advance object.
             */
            Order* clone() const override;

            /**
             * @brief Validate the order.
             * @return True if the order follows game rules:
             * - The source belongs to the player
             * - The player has enough armies
             * - The source and target are adjacent.
             */
            bool validate() const override;

            /**
             * @brief Execute the order.
             * If valid, moves armies between territories.
             * If target is enemy territory, resolves battle.
             * If target is friendly, reinforces it.
             */
            void execute() override;
    };

    /**
     * @class Bomb
     * @brief Represents a Bomb order.
     * 
     * A Bomb order removes half of the armies from an enemy territory.
     * This order can only target territories not owned by the issuing player.
     */
    class Bomb : public Order {
        
        private:

            // Bomb Attributes
            Player* issuer;
            Territory* target;

        public:

            //-- Constructors, Destructor, Copy Constructor, Assignment Operator, Stream Insertion Operator --//

            /**
             * @brief Parameterized constructor.
             * @param p The player issuing the order.
             * @param t The target enemy territory.
             */
            Bomb(Player* p, Territory* t);

            /**
             * @brief Destructor
             */
            ~Bomb();

            /**
             * @brief Copy constructor.
             * @param other The Bomb order to copy from.
             */
            Bomb(const Bomb& other);

            /**
             * @brief Assignment operator.
             * @param other The Bomb order to assign from.
             * @return Reference to this Bomb order.
             */
            Bomb& operator=(const Bomb& other);

            /**
             * @brief Print details specific to a Bomb order.
             * @param os The output stream.
             */
            void print(std::ostream& os) const override;

            // ---- Accessors and Mutators ---- //

            /**
             * @brief Get the issuing player of this order.
             * @return Pointer to the Player who issued the order.
             */
            Player* getIssuer() const;

            /**
             * @brief Set the issuing player of this order.
             * @param p Pointer to the Player issuing the order.
             */
            void setIssuer(Player* p);

            /**
             * @brief Get the target territory of this order.
             * @return Pointer to the target Territory.
             */
            Territory* getTarget() const;

            /**
             * @brief Set the target territory of this order.
             * @param t Pointer to the new target Territory.
             */
            void setTarget(Territory* t);

            //--- Overridden Class Methods ---//

            /**
             * @brief Polymorphic copy constructor.
             * @return A pointer to a new Bomb object.
             */
            Order* clone() const override;

            /**
             * @brief Validate the order.
             * @return True if the order follows game rules:
             * - The target does not belong to the issuing player.
             */
            bool validate() const override;

            /**
             * @brief Execute the order.
             * If valid, removes half the armies from the target territory.
             */
            void execute() override;

    };

    /**
     * @class Blockade
     * @brief Represents a Blockade order.
     * 
     * A Blockade order doubles the armies on one of the player’s territories
     * and transfers ownership of that territory to the Neutral player.
     */
    class Blockade : public Order {
        
        private:

            // Blockade Attributes
            Player* issuer;          ///< The player issuing the order
            Territory* target;       ///< The target territory to be blockaded
            Player* neutralPlayer;   ///< The Neutral player who will take ownership after blockade

        public:

            //-- Constructors, Destructor, Copy Constructor, Assignment Operator, Stream Insertion Operator --//

            /**
             * @brief Parameterized constructor.
             * @param p The player issuing the order.
             * @param t The target territory to blockade.
             * @param neutral The Neutral player who will take ownership.
             */
            Blockade(Player* p, Territory* t, Player* neutral);

            /**
             * @brief Destructor
             */
            ~Blockade();

            /**
             * @brief Copy constructor.
             * @param other The Blockade order to copy from.
             */
            Blockade(const Blockade& other);

            /**
             * @brief Assignment operator.
             * @param other The Blockade order to assign from.
             * @return Reference to this Blockade order.
             */
            Blockade& operator=(const Blockade& other);

            /**
             * @brief Print details specific to a Blockade order.
             * @param os The output stream.
             */
            void print(std::ostream& os) const override;

            // ---- Accessors and Mutators ---- //

            /**
             * @brief Get the issuing player of this order.
             * @return Pointer to the Player who issued the order.
             */
            Player* getIssuer() const;

            /**
             * @brief Set the issuing player of this order.
             * @param p Pointer to the Player issuing the order.
             */
            void setIssuer(Player* p);

            /**
             * @brief Get the target territory of this order.
             * @return Pointer to the target Territory.
             */
            Territory* getTarget() const;

            /**
             * @brief Set the target territory of this order.
             * @param t Pointer to the new target Territory.
             */
            void setTarget(Territory* t);

            /**
             * @brief Get the Neutral player who will take ownership.
             * @return Pointer to the Neutral Player.
             */
            Player* getNeutralPlayer() const;

            /**
             * @brief Set the Neutral player who will take ownership.
             * @param neutral Pointer to the Neutral Player.
             */
            void setNeutralPlayer(Player* neutral);

            //--- Overridden Class Methods ---//

            /**
             * @brief Polymorphic copy constructor.
             * @return A pointer to a new Blockade object.
             */
            Order* clone() const override;

            /**
             * @brief Validate the order.
             * @return True if the order follows game rules:
             *         - The target territory belongs to the issuing player.
             */
            bool validate() const override;

            /**
             * @brief Execute the order.
             * If valid, doubles the number of armies on the target territory
             * and transfers its ownership to the Neutral player.
             */
            void execute() override;
    };


    /**
     * @class Airlift
     * @brief Represents an Airlift order.
     * 
     * An Airlift order moves armies between two owned territories,
     * regardless of adjacency.
     */
    class Airlift : public Order {
        
        private:

            // Airlift Attributes
            Player* issuer;
            Territory* source;
            Territory* target;
            int numArmies;

        public:

            //-- Constructors, Destructor, Copy Constructor, Assignment Operator, Stream Insertion Operator --//

            /**
             * @brief Parameterized constructor.
             * @param p The player issuing the order.
             * @param s The source territory.
             * @param t The target territory.
             * @param armies The number of armies to airlift.
             */
            Airlift(Player* p, Territory* s, Territory* t, int armies);

            /**
             * @brief Destructor
             */
            ~Airlift();

            /**
             * @brief Copy constructor.
             * @param other The Airlift order to copy from.
             */
            Airlift(const Airlift& other);

            /**
             * @brief Assignment operator.
             * @param other The Airlift order to assign from.
             * @return Reference to this Airlift order.
             */
            Airlift& operator=(const Airlift& other);

            /**
             * @brief Print details specific to an Airlift order.
             * @param os The output stream.
             */
            void print(std::ostream& os) const override;

            // ---- Accessors and Mutators ---- //

            /**
             * @brief Get the issuing player of this order.
             * @return Pointer to the Player who issued the order.
             */
            Player* getIssuer() const;

            /**
             * @brief Set the issuing player of this order.
             * @param p Pointer to the Player issuing the order.
             */
            void setIssuer(Player* p);

            /**
             * @brief Get the source territory of this order.
             * @return Pointer to the source Territory.
             */
            Territory* getSource() const;

            /**
             * @brief Set the source territory of this order.
             * @param s Pointer to the new source Territory.
             */
            void setSource(Territory* s);

            /**
             * @brief Get the target territory of this order.
             * @return Pointer to the target Territory.
             */
            Territory* getTarget() const;

            /**
             * @brief Set the target territory of this order.
             * @param t Pointer to the new target Territory.
             */
            void setTarget(Territory* t);

            /**
             * @brief Get the number of armies to airlift.
             * @return The number of armies as an integer.
             */
            int getNumArmies() const;

            /**
             * @brief Set the number of armies to airlift.
             * @param armies The new number of armies.
             */
            void setNumArmies(int armies);

            //--- Overridden Class Methods ---//

            /**
             * @brief Polymorphic copy constructor.
             * @return A pointer to a new Airlift object.
             */
            Order* clone() const override;

            /**
             * @brief Validate the order.
             * @return True if the order follows game rules:
             * - Both source and target territories belong to the player.
             * - The source has enough armies to allow for redployment of armies.
             */
            bool validate() const override;

            /**
             * @brief Execute the order.
             * If valid, moves the specified number of armies
             * from the source territory to the target territory,
             * regardless of adjacency.
             */
            void execute() override;

    };

    /**
     * @class Negotiate
     * @brief Represents a Negotiate order.
     * 
     * A Negotiate order establishes temporary peace between two players,
     * preventing them from attacking each other for the remainder of the turn.
     * 
     * This order is binding and does not require acceptance by the target player.
     */
    class Negotiate : public Order {
        
        private:

            //-- Negotiate Attributes --//
            Player* issuer;
            Player* targetPlayer;

        public:

            //-- Constructors, Destructor, Copy Constructor, Assignment Operator, Stream Insertion Operator --//

            /**
             * @brief Parameterized constructor.
             * @param p The player issuing the order.
             * @param other The target player with whom peace is established.
             */
            Negotiate(Player* p, Player* other);

            /**
             * @brief Destructor
             */
            ~Negotiate();

            /**
             * @brief Copy constructor.
             * @param other The Negotiate order to copy from.
             */
            Negotiate(const Negotiate& other);

            /**
             * @brief Assignment operator.
             * @param other The Negotiate order to assign from.
             * @return Reference to this Negotiate order.
             */
            Negotiate& operator=(const Negotiate& other);

            /**
             * @brief Print details specific to a Negotiate order.
             * @param os The output stream.
             */
            void print(std::ostream& os) const override;

            // ---- Accessors and Mutators ---- //

            /**
             * @brief Get the issuing player of this order.
             * @return Pointer to the Player who issued the order.
             */
            Player* getIssuer() const;

            /**
             * @brief Set the issuing player of this order.
             * @param p Pointer to the Player issuing the order.
             */
            void setIssuer(Player* p);

            /**
             * @brief Get the target player of this order.
             * @return Pointer to the target Player.
             */
            Player* getTargetPlayer() const;

            /**
             * @brief Set the target player of this order.
             * @param other Pointer to the new target Player.
             */
            void setTargetPlayer(Player* other);

            //--- Overridden Class Methods ---//

            /**
             * @brief Polymorphic copy constructor.
             * @return A pointer to a new Negotiate object.
             */
            Order* clone() const override;

            /**
             * @brief Validate the order.
             * @return True if the target player is valid and not the same as the issuer.
             */
            bool validate() const override;

            /**
             * @brief Execute the order.
             * 
             * If valid, this order modifies both players’ state by adding each player
             * to the other’s list of neutral enemies (via Player::addNeutralEnemy()).
             * 
             * Later, when toAttack() is called, territories belonging to neutral enemies
             * will not appear in the list of valid attack options, preventing
             * attacks between the two players for the rest of the turn.
             */

            void execute() override;

    };

    /*------------------------------------------ORDERLIST CLASS--------------------------------------------------*/
    
    /**
     * @class OrderList
     * @brief Manages a collection of Order objects for a player.
     */
    class OrderList {
        
        private:

            //Normal container for orders
            std::vector<Order*>* orders;

        public:

            //-- Constructors, Destructor, Copy Constructor, Assignment Operator, Stream Insertion Operator --//

            /**
             * @brief Default constructor.
             * 
             * Initializes an empty list of orders.
             */
            OrderList();

            /**
             * @brief Destructor.
             * 
             * Cleans up all dynamically allocated Orders in the list.
             */
            ~OrderList();

            /**
             * @brief Copy constructor.
             * 
             * Creates a deep copy of another OrderList, duplicating all Orders.
             * @param other The OrderList to copy from.
             */
            OrderList(const OrderList& other);

            /**
             * @brief Assignment operator.
             * 
             * Assigns another OrderList to this one by performing a deep copy
             * of all Order objects.
             * @param other The OrderList to assign from.
             * @return Reference to this OrderList.
             */
            OrderList& operator=(const OrderList& other);

            /**
             * @brief Stream insertion operator.
             * 
             * Outputs all Orders in the list to the given stream, showing
             * their type and effect strings.
             * @param os The output stream.
             * @param ol The OrderList to output.
             * @return Reference to the output stream.
             */
            friend std::ostream& operator<<(std::ostream& os, const OrderList& ol);

            //-- Accessors and Mutators --//

            /**
             * @brief Get the vector of Orders.
             * @return A pointer to the vector of Order pointers.
             */
            std::vector<Order*>* getOrders() const;

            /**
             * @brief Set the vector of Orders.
             * @param newOrders A pointer to a new vector of Order pointers.
             */
            void setOrders(const vector<Order*>* newOrders);


            //-- Class Methods --//

            /**
             * @brief Add a new Order to the list.
             * 
             * The order is appended to the end of the list.
             * @param o Pointer to the Order to add.
             */
            void addOrder(Order* o);

            /**
             * @brief Remove an Order from the list.
             * 
             * Deletes and removes the Order at the specified index.
             * @param index The position of the Order to remove.
             */
            void remove(int index);

            /**
             * @brief Move an Order within the list.
             * 
             * Relocates an Order from one position to another.
             * @param from The index of the Order to move.
             * @param to The new index where the Order will be placed.
             */
            void move(int from, int to);
            
    };

}