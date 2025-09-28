#pragma once


#include <algorithm>
#include <string>
#include <vector>
#include <unordered_map>
#include <iostream>

#include "../Map/Map.h"
#include "../Card/Card.h"
#include "../Order/Order.h"

namespace WarzoneOrder { class OrderList; }  //Forward declaration for order list

namespace WarzonePlayer {

    using WarzoneMap::Territory;
    using WarzoneCard::Hand;
    using WarzoneOrder::TimeUtil;
    using WarzoneOrder::OrderList;
    using WarzoneOrder::Order;

    /**
     * @brief Manages a Player's owned territories using both a vector (for iteration), and an unordered_map (for fast lookups).
     */
    class PlayerTerrContainer {

        private:

            vector<Territory*> territories; //Iteration
            unordered_map<string, Territory*> territoryIndex; //For O(1) lookup by ID

        public:

            //-- Constructors, Destructor, Copy Constructor, Assignment Operator, Stream Insertion Operator --//

            /** 
             * @brief Default constructor 
             */
            PlayerTerrContainer();

            /** 
             * @brief Pramaterized constructor
             * @param territories Vector of territories, for iteration
             * @param territoryIndex Hashmap of territories, for indexing
             */
            PlayerTerrContainer(vector<Territory*> territories, unordered_map<string, Territory*> territoryIndex);
            
            /**
             * @brief Destructor
             */
            ~PlayerTerrContainer();

            /**
             * @brief Copy constructor
             * @param other PlayerTerrContainer object to copy from
            */
            PlayerTerrContainer(const PlayerTerrContainer& other);

            /**
             * @brief Assignment operator
             * @param other PlayerTerrContainer object to assign from
             * @return Reference to this PlayerTerrContainer
             */
            PlayerTerrContainer& operator=(const PlayerTerrContainer& other);

            /**
             * @brief Stream insertion operator
             * @param os Output stream
             * @param PlayerTerrContainer PlayerTerrContainer to output
             * @return Reference to output stream
             */
            friend ostream& operator<<(ostream& os, const PlayerTerrContainer& pc);

            //-- Accesors and Mutators --//

            /**
             * @brief Accessor for territories vector
             * @return const reference to vector of Territory* 
             */
            const vector<Territory*>& getTerritories() const;

            /**
             * @brief Mutator for territories vector
             * @param newTerritories vector of Territory* to set
             * @param newOwner Player owner for the new territories
             */
            void setTerritories(const vector<Territory*>& newTerritories, Player* newOwner);

            /**
             * @brief Accessor for territoryIndex map
             * @return const reference to unordered_map of Territory ID -> Territory*
             */
            const unordered_map<string, Territory*>& getTerritoryIndex() const;

            /**
             * @brief Mutator for territoryIndex map
             * @param newIndex unordered_map of Territory ID -> Territory* to set
             * @param newOwner Player owner for the new territories
             */
            void setTerritoryIndex(const unordered_map<string, Territory*>& newIndex, Player* newOwner);


            //-- Class Methods --//

            /**
             * @brief Adds a territory if not already owned.
             * @param t Territory to be add
             * @param newOwner Owner who will be assigned this territory
             */
            void addTerritory(Territory* t, Player* newOwner);

            /**
             * @brief Removes a territory if owned.
             * @param t Territory to be removed
             * @param oldOwner Owner who will lose this territory
             */
            void removeTerritory(Territory* t);

            /**
             * @brief Check if a territory is owned by this player.
             * @return true if owned, false otherwise.
             */
            bool owns(Territory* t) const;

            /**
             * @brief Clears all owned territories.
             */
            void clear();

            /**
             * @brief Number of territories owned.
             */
            size_t size() const;
            
    };

    /**
     * @brief The Player class represents a Warzone player.
     */
    class Player {
    
        private:

            //-- Player Attributes --//
            string playerName;
            vector<string> neutralEnemies;
            PlayerTerrContainer ownedTerritories;
            Hand* playerHand;
            OrderList* playerOrders;
            bool generateCardThisTurn;
      
        public:

            //-- Constructors, Destructor, Copy Constructor, Assignment Operator, Stream Insertion Operator --//

            /**
             * @brief Default constructor. Creates an empty player.
             */
            Player();

            /**
             * @brief Constructs a player with a given name.
             * @param name The name of the player.
             */
            Player(const string& name);

            /**
             * @brief Constructs a player with a given name, hand, and order list. Useful for game engine
             * @param name The name of the player.
             * @param hand Pointer to the player's hand of cards.
             * @param orders Pointer to the player's order list.
             */
            Player(const string& name, Hand* hand, OrderList* orders);

            /**
             * @brief Constructs a player with a given name, list of territories, hand, and order list.
             * @param name The name of the player.
             * @param neutralEnemies The names of enemies that the player may not attack
             * @param ownedTerritories The list of owned territories
             * @param hand Pointer to the player's hand of cards.
             * @param orders Pointer to the player's order list.
             * @param generateCardThisTurn Boolean flag to determine whether a card should be added to the player's hand
             */
            Player(const string& name, vector<string> neutralEnemies, PlayerTerrContainer ownedTerritories, Hand* hand, OrderList* orders, bool generateCard);

            /**
             * @brief Destructor. Cleans up player's hand and orders if allocated.
             */
            ~Player();

            /**
             * @brief Copy constructor.
             * @param other Another Player object to copy from.
             */
            Player(const Player& other);

            /**
             * @brief Assignment operator.
             * @param other Another Player object to assign from.
             * @return Reference to this Player.
             */
            Player& operator=(const Player& other);

            /**
             * @brief Stream insertion operator for Player.
             * @param os Output stream.
             * @param p Player to output.
             * @return Reference to the output stream.
             */
            friend ostream& operator<<(ostream& os, const Player& p);

            //-- Accessors and Mutators --//

            /**
             * @brief Accessor for playerName.
             * @return Player's name as a const reference.
             */
            const string& getPlayerName() const;

            /**
             * @brief Mutator for player's name.
             * @param playerName The new name for the player.
             */
            void setPlayerName(const string& playerName);

            /**
             * @brief Accessor for neutralEnemies.
             * @return All neutral enemies as a const reference.
             */
            const vector<string>& getNeutralEnemies() const;

            /**
             * @brief Mutator for neutralEnemies.
             * @param playerName The new name for the player.
             */
            void setNeutralEnemies(const vector<string>& neutralEnemies);

            /**
             * @brief Accessor for owned territories container.
             * @return Const reference to PlayerTerrContainer.
             */
            const PlayerTerrContainer& getOwnedTerritories() const;

            /**
             * @brief Mutator for owned territories container.
             * @param newTerritories PlayerTerrContainer to set.
             */
            void setOwnedTerritories(const PlayerTerrContainer& newTerritories);

            /**
             * @brief Accessor for player's hand.
             * @return Pointer to Hand.
             */
            Hand* getHand() const;

            /**
             * @brief Mutator for player's hand.
             * @param newHand Pointer to a Hand.
             */
            void setHand(Hand* newHand);

            /**
             * @brief Accessor for player's order list.
             * @return Pointer to OrderList.
             */
            OrderList* getPlayerOrders() const;

            /**
             * @brief Mutator for player's order list.
             * @param newOrders Pointer to an OrderList.
             */
            void setPlayerOrders(OrderList* newOrders);

            /**
             * @brief Accessor for generateCardThisTurn flag.
             * @return True if the player should generate a card this turn, false otherwise.
             */
            bool getGenerateCardThisTurn() const;

            /**
             * @brief Mutator for generateCardThisTurn flag.
             * @param flag Boolean value to set.
             */
            void setGenerateCardThisTurn(bool flag);

            //-- Class Methods --//

            /**
             * @brief Returns a list of territories that the player may choose to attack.
             * @return Vector of Territory pointers.
             */
            vector<Territory*> toAttack();

            /**
             * @brief Prints out the attack possibilites for a player
             */
            void toAttackPrint() const;


            /**
             * @brief Returns a list of territories that the player may choose to defend.
             * @return Vector of Territory pointers.
             */
            vector<Territory*> toDefend();

             /**
             * @brief Prints out the defence possibilites for a player
             */
            void toDefendPrint() const;

            /**
             * @brief Creates an order and adds it to the player’s order list.
             */
            void issueOrder();

            /**
             * @brief Adds a territory to the player’s owned territories.
             * @param territory Pointer to the territory to add.
             */
            void addOwnedTerritories(WarzoneMap::Territory* territory);

            /**
             * @brief Removes a territory from the player’s owned territories.
             * @param territory Pointer to the territory to remove.
             */
            void removeOwnedTerritories(WarzoneMap::Territory* territory);

            /**
             * @brief Add a single neutral enemy (negotiated player).
             * @param enemyName The name of the neutral enemy player.
             */
            void addNeutralEnemy(const string& enemyName);

            /**
             * @brief Remove a neutral enemy if present.
             * @param enemyName The name of the enemy to remove.
             */
            void removeNeutralEnemy(const string& enemyName);

    };

}