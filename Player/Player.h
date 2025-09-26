#pragma once

#include <vector>
#include <unordered_map>
#include <string>
#include <algorithm>
#include <iostream>
#include <map>

// Map dependencies
#include "../Map/Map.h"

// Game dependencies
#include "../Card/Card.h"
#include "../Order/Order.h"


namespace WarzonePlayer {

    using namespace std;
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
        Player();
        Player(const string& name);
        Player(const string& name, Hand* hand, Order* orders);
        Player(const string& name, PlayerTerrContainer ownedTerritories, Hand* hand, Order* orders);
        ~Player();

        Player(const Player& other);
        Player& operator=(const Player& other);

        friend ostream& operator<<(ostream& os, const Player& p);

        //-- Accessors and Mutators --//
        const string& getPlayerName() const;
        void setPlayerName(const string& playerName);

        const PlayerTerrContainer& getOwnedTerritories() const;
        void setOwnedTerritories(const PlayerTerrContainer& newTerritories);

        Hand* getHand() const;
        void setHand(Hand* newHand);

        Order* getOrders() const;
        void setOrders(Order* newOrders);

        //-- Class Methods --//
        vector<Territory*> toAttack();
        vector<Territory*> toDefend();
        void issueOrders();

        void addOwnedTerritories(Territory* territory);
        void removeOwnedTerritories(Territory* territory);
    };

} // namespace WarzonePlayer
