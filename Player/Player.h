#pragma once
#include "Map/Map.h"
#include "Card/Card.h"
#include "Order/Order.h"

#include <algorithm>

namespace WarzonePlayer {

    using WarzoneMap::Territory;
    using WarzoneCard::Hand;
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
             */
            void setTerritories(const vector<Territory*>& newTerritories);

            /**
             * @brief Accessor for territoryIndex map
             * @return const reference to unordered_map of Territory ID -> Territory*
             */
            const unordered_map<string, Territory*>& getTerritoryIndex() const;

            /**
             * @brief Mutator for territoryIndex map
             * @param newIndex unordered_map of Territory ID -> Territory* to set
             */
            void setTerritoryIndex(const unordered_map<string, Territory*>& newIndex);


            //-- Class Methods --//

            /**
             * @brief Adds a territory if not already owned.
             */
            void addTerritory(Territory* t);

            /**
             * @brief Removes a territory if owned.
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
          PlayerTerrContainer ownedTerritories;
          Hand* playerHand;
          Order* playerOrders;
      
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
          Player(const string& name, Hand* hand, Order* orders);

          /**
           * @brief Constructs a player with a given name, list of territories, hand, and order list.
           * @param name The name of the player.
           * @param hand Pointer to the player's hand of cards.
           * @param orders Pointer to the player's order list.
           */
          Player(const string& name, PlayerTerrContainer ownedTerritories, Hand* hand, Order* orders);

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
          Order* getOrders() const;

          /**
           * @brief Mutator for player's order list.
           * @param newOrders Pointer to an OrderList.
           */
          void setOrders(Order* newOrders);

          //-- Class Methods --//

          /**
           * @brief Returns a list of territories that the player may choose to attack.
           * @return Vector of Territory pointers.
           */
          vector<WarzoneMap::Territory*> toAttack();

          /**
           * @brief Returns a list of territories that the player may choose to defend.
           * @return Vector of Territory pointers.
           */
          vector<WarzoneMap::Territory*> toDefend();

          /**
           * @brief Creates an order and adds it to the player’s order list.
           */
          void issueOrders();

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

    };

}