#pragma once


#include <algorithm>
#include <string>
#include <vector>
#include <unordered_map>
#include <iostream>
#include <queue>

#include "../Map/Map.h"
#include "../Card/Card.h"
#include "../Order/Order.h"

namespace WarzoneOrder { class OrderList; }  //Forward declaration for order list


namespace WarzonePlayer {

    using WarzoneMap::Territory;
    using WarzoneMap::Continent;
    using WarzoneCard::Hand;
    using WarzoneOrder::TimeUtil;
    using WarzoneOrder::OrderList;
    using WarzoneOrder::Order;
    using WarzoneCard::Deck;
    using WarzoneCard::Hand;
    using WarzoneCard::Card;

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
            vector<Player*> neutralEnemies;
            PlayerTerrContainer ownedTerritories;
            Hand* playerHand;
            OrderList* playerOrders;
            bool generateCardThisTurn;
            int reinforcementPool;

            //-- Game Logic Info --/
            unordered_map<Continent*, long long> continentLookupTablePlayer;
      
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
            Player(const string& name, const unordered_map<Continent*, long long>& emptyHashMap);

            /**
             * @brief Constructs a player with a given name, list of territories, hand, and order list.
             * @param name The name of the player.
             * @param neutralEnemies The list of players that the player may not attack
             * @param ownedTerritories The list of owned territories
             * @param hand Pointer to the player's hand of cards.
             * @param orders Pointer to the player's order list.
             * @param generateCardThisTurn Boolean flag to determine whether a card should be added to the player's hand
             * @param reinforcementPool Keep track of how many armies a player can deploy at one time
             * @param emptyHashMap Hashmap used to keep track of player progress towards continent capture
             */
            Player(const string& name, vector<Player*> neutralEnemies, PlayerTerrContainer ownedTerritories, Hand* hand, OrderList* orders, 
                   bool generateCard, int reinforcementPool, const unordered_map<Continent*, long long>& emptyHashMap);

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
            const vector<Player*>& getNeutralEnemies() const;

            /**
             * @brief Mutator for neutralEnemies.
             * @param neutralEnemies List of neutral enemies
             */
            void setNeutralEnemies(const vector<Player*>& neutralEnemies);

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

            /**
             * @brief Accessor for Reinforcement pool.
             * @return The number of armies a player can call to deploy.
             */
            int getReinforcementPool() const;

            /**
             * @brief Mutator for Reinforcement pool.
             * @param pool The number of armies a player can use to deploy.
             */
            void setReinforcementPool(int pool);

            /**
             * @brief Accessor for continentLookupTablePlayer.
             * @return Const reference to the player's continent lookup table.
             */
            const unordered_map<WarzoneMap::Continent*, long long>& getContinentLookupTablePlayer() const;

            /**
             * @brief Mutator for continentLookupTablePlayer.
             * @param newTable The new continent lookup table to assign.
             */
            void setContinentLookupTablePlayer(const unordered_map<WarzoneMap::Continent*, long long>& newTable);

            //-- Class Methods --//

            /**
             * @brief Determines the optimal attack targets for each owned territory.
             *
             * For every territory owned by the player, this method evaluates adjacent enemy
             * territories and selects the weakest as its preferred attack target. Territories
             * without any valid enemy neighbors are mapped to nullptr.
             * 
             * @return An unordered_map where each key is one of the player’s owned territories,
             *         and each value is the weakest valid enemy territory to attack (or nullptr
             *         if none are available).
             */
            unordered_map<Territory*, Territory*> toAttack();


            /**
             * @brief Generates a string representation of toAttack
             * @return A string representationof who the current player may attack
             */
            string toAttackString();


            /**
            * @brief Builds a defensive  map for all owned territories.
            *
            * Each territory is mapped to the next territory that should receive defensive
            * reinforcements, forming a directed reinforcement chain toward the enemy front.
            * 
            * - Border territories (adjacent to enemies) have a value of nullptr.
            * - Rear territories point to the closest owned territory leading toward the border.
            *
            * This structure allows direct generation of defensive Advance orders by moving
            * troops along these precomputed reinforcement paths.
            *
            * @return An unordered_map where each key is a territory owned by the player,
            *         and each value is the territory that should receive reinforcements next.
            */
            unordered_map<Territory*, Territory*> toDefend();



             /**
             * @brief Generates a string representation of toDefend
             * @return A string representation of what territories the current player may defend 
             */
            string toDefendString();

            /**
             * @brief Method to handle deployment logic
             *  This method identifies all enemy-adjacent territories (frontlines) using the defensive routing map returned
             * by toDefend(). Armies are distributed primarily among these territories, with weaker ones prioritized first.
             * 
             * Distribution logic:
             * - Evenly divide remaining armies among all frontlines (rounded up).
             * - Randomly vary the deployed amount slightly for unpredictability.
             * - Ensure that each deployment does not exceed the remaining pool.
             *
             * @param output Stream used for logging actions (shared with issueOrder()).
             * @param surpressOutput If true, console/log output is suppressed
             */
            void deployReinforcements(ostringstream& output, bool surpressOutput);

            /**
             * @brief Issues Advance orders to attack neighboring enemy territories.
             *
             * Iterates through all border territories owned by the player and identifies enemy-adjacent
             *  targets. Territories are ranked and evaluated for attack viability before creating Advance
             * orders. Attacks only occur when the enemy has weaker or comparable defenses. (Player must have 7/6* more armies on a territory). 
             *
             * @param output Shared log stream to record order actions.
             * @param surpressOutput If true, suppresses console/log output.
             * @param neutralPlayer Pointer to the set neutral player
             */
            void issueAttackOrders(ostringstream& output, bool surpressOutput, Player* neutralPlayer);

            /**
             * @brief Issues Advance orders to reinforce frontlines based on defensive layering.
             *
             * This method retrieves the defense propagation map from toDefend(), which represents
             * how owned territories are layered by proximity to enemy borders. Each interior
             * territory points toward the next territory closer to the frontlines.
             *
             * For every territory with available armies (more than one), the player issues
             * an Advance order to send troops toward its designated target territory closer
             * to the frontlines. 
             *
             * @param output Reference to the ostringstream used for collecting log outputs.
             * @param surpressOutput If true, disables console output; otherwise, logs actions.
             */
            void issueDefendOrders(ostringstream& output, bool surpressOutput);

            /**
             * @brief Issues a random card-based order if the player holds a valid card.
             *
             * This method checks the player’s hand for available cards of types:
             * Bomb, Blockade, Airlift, and Negotiate. It verifies logical validity for
             * each (via canIssueCardOrder()), randomly selects one, constructs the
             * corresponding Order subclass, and adds it to the player’s order list.
             *
             * Used cards are removed from the player’s hand and returned to the Deck.
             *
             * @param output Reference to the ostringstream used for collecting log outputs.
             * @param surpressOutput If true, disables console output; otherwise logs actions.
             * @param gameDeck Pointer to the game’s Deck object for returning used cards.
             */
            void issueCardOrders(ostringstream& output, bool surpressOutput, Deck* gameDeck);


            /**
             * @brief Creates an empty order, and adds it to the player’s order list.
             * Also deploys a random amount of armies to territories
             * @param surpressOutput reduce the amount of information saved during executions
             * @param gameDeck Deck pointer to return issued card to
             * @param neutralPlayer Pointer to the set neutral player
             * @return A string to detail the execution of the method
             */
            string issueOrder(bool surpressOutput, Deck* gameDeck, Player* neutralPlayer);

            /**
             * @brief Check if the player can issue any orders found in a card.
             * @return True if the player can issue orders, false otherwise.
             * 
            */
            bool canIssueCardOrder(int orderType) const;

            /**
             * @brief Helper method to clear a player's truce list at the end of every turn.
             */
            void clearNeutralEnemies();

            /**
             * @brief Adds a territory to the player’s owned territories.
             * @param territory Pointer to the territory to add.
             */
            void addOwnedTerritories(Territory* territory);

            /**
             * @brief Removes a territory from the player’s owned territories.
             * @param territory Pointer to the territory to remove.
             */
            void removeOwnedTerritories(Territory* territory);

            /**
             * @brief Add a single neutral enemy (negotiated player).
             * @param enemyName The neutral enemy player.
             */
            void addNeutralEnemy(Player* enemyName);

            /**
             * @brief Remove a neutral enemy if present.
             * @param enemyName The neutral enemy to remove.
             */
            void removeNeutralEnemy(Player* enemyName);

            /**
             * @brief Returns all of the player's territories that are adjacent to at least one enemy territory.
             * 
             * A territory is considered adjacent to an enemy if any of its neighboring territories
             * are owned by another player (not this one), regardless of truces or neutrality.
             * 
             * @return A vector of pointers to territories adjacent to enemy territories.
             */
            vector<Territory*> getTerritoriesAdjacentToEnemy() const;

            /**
             * @brief Get a list of territories that have many armies and can be used as sources for advance orders.
             * A territory is considered a valid source if it has more than one army.
             * @return Vector of Territory pointers that can be used as sources.
             */
            vector<Territory*> getSourcesWithManyArmies() const;

            /**
             * @brief Get a list of territories that can be bombed or blockaded.
             * These are territories owned by the player that have at least one non-owned neighbor.
             * @return Vector of Territory pointers that are valid targets.
             */
            vector<Territory*> getBombCandidates() const;

            /**
             * @brief Check if this player controls a given continent.
             * @param continentSums Reference to the ground-truth continent lookup table, from the game Map.
             * @param cont Pointer to the continent to check.
             * @return True if the player's sum for the continent matches the reference sum.
             */
            bool controlsContinent(const unordered_map<Continent*, long long>& continentSums, Continent* cont) const;

            /**
             * @brief Check if this player controls all continents.
             * @param continentSums Reference to the ground-truth continent lookup table, from the game Map.
             * @return True if the player owns all continents, false otherwise.
             */
            bool controlsMap(const unordered_map<Continent*, long long>& continentSums) const;

    };

}