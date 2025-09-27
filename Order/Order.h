#pragma once

#include <vector>
#include <string>
#include <iostream>

#include "../Map/Map.h"
#include "../Player/Player.h"

using WarzoneMap::Territory;
using WarzonePlayer::Player;
using std::string;

namespace WarzoneOrder {

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
    class Order {

        protected:
        
            OrderType type;
            string effect;

        public:
            Order(OrderType t);
            Order(const Order& other);
            virtual ~Order() = default;

            virtual Order* clone() const = 0;
            virtual bool validate() const = 0;
            virtual void execute() = 0;

            Order& operator=(const Order& other);
            friend std::ostream& operator<<(std::ostream& os, const Order& order);

            OrderType getType() const;
    };

    /*------------------------------------------ SINGLE ORDER SUB-CLASSES------------------------------------------------*/
    class Deploy : public Order {
        public:
            Deploy();                          // automatically sets type = Deploy
            Deploy(const Deploy& other);
            Order* clone() const override;
            bool validate() const override;
            void execute() override;
    };

    class Advance : public Order {
        public:
            Advance();                         // automatically sets type = Advance
            Advance(const Advance& other);
            Order* clone() const override;
            bool validate() const override;
            void execute() override;
    };

    class Bomb : public Order {
        public:
            Bomb();                            // automatically sets type = Bomb
            Bomb(const Bomb& other);
            Order* clone() const override;
            bool validate() const override;
            void execute() override;
    };

    class Blockade : public Order {
        public:
            Blockade();                        // automatically sets type = Blockade
            Blockade(const Blockade& other);
            Order* clone() const override;
            bool validate() const override;
            void execute() override;
    };

    class Airlift : public Order {
        public:
            Airlift();                         // automatically sets type = Airlift
            Airlift(const Airlift& other);
            Order* clone() const override;
            bool validate() const override;
            void execute() override;
    };

    class Negotiate : public Order {
        public:
            Negotiate();                       // automatically sets type = Negotiate
            Negotiate(const Negotiate& other);
            Order* clone() const override;
            bool validate() const override;
            void execute() override;
    };

    /*------------------------------------------ORDERLIST CLASS--------------------------------------------------*/
    class OrderList {
        private:
            std::vector<Order*>* orders;

        public:
            OrderList();
            ~OrderList();
            OrderList(const OrderList& other);
            OrderList& operator=(const OrderList& other);

            void addOrder(Order* o);
            void remove(int index);
            void move(int from, int to);

            friend std::ostream& operator<<(std::ostream& os, const OrderList& ol);
    };

    /*-------------------------------------------TEST-DRIVER FUNCTION---------------------------------------------*/
    void testOrdersList();

}
