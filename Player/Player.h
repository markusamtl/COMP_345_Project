#pragma once

#include "player_territory_container/PlayerTerrContainer.h"
#include "../card/Hand.h"
#include "../order/order_implementation/OrderList.h"
#include <iostream>
#include <string>
#include <map>
#include <vector>

using namespace std;

/**
 * @brief The Player class represents a Warzone player.
 * 
 * A Player has:
 * - A name
 * - A collection of owned territories (PlayerTerrContainer)
 * - A hand of cards
 * - A list of orders
 * 
 * The Player can decide which territories to defend/attack and issue orders during their turn.
 */
class Player {

    private:

        //-- Player Attributes --//

        string playerName;
        PlayerTerrContainer ownedTerritories;
        Hand* playerHand;
        OrderList* playerOrders;

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
         * @param hand Pointer to the player's hand of cards.
         * @param orders Pointer to the player's order list.
         */
        Player(const string& name, PlayerTerrContainer ownedTerritories, Hand* hand, OrderList* orders);

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
        OrderList* getOrders() const;

        /**
         * @brief Mutator for player's order list.
         * @param newOrders Pointer to an OrderList.
         */
        void setOrders(OrderList* newOrders);

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