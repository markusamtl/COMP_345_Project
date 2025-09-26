#pragma once

#include <vector>

#include "../Map/Map.h"
#include "../Player/Player.h"

using WarzoneMap::Territory;
using WarzonePlayer::Player;


namespace WarzoneOrder {

    //an enum class so each order object can store its type, and will help with switch statements (no need to use strings), etc 
    enum class OrderType {
        Deploy, Advance, Bomb, Blockade, Airlift, Negotiate 
    };

    /*------------------------------------------ORDER SUPER-CLASS------------------------------------------------*/
    class Order {
        
        protected: //and not private, so the subclasses can access these attributes 
        OrderType type;

        public:

        //contructor, copy constructor 
        Order(OrderType t);
        Order(const Order& other);

        //polymorphic clone
        virtual Order* clone() const =0;

        //pure virtual functions
        virtual bool validate() const =0;
        virtual void execute() =0;

        //operator overloads
        Order& operator=(const Order& other);
        friend std::ostream& operator<<(std::ostream& os, const Order& order);
    };

    /*------------------------------------------ SINGLE ORDER SUB-CLASSES------------------------------------------------*/

    //DEPLOY: put number of army units on target territory
    class Deploy : public Order {

        //do we need a player attribute or a player container attribute for the player issuing the order?
        Territory* target;
        int numArmies;

        public:

        //function overrides
        bool validate() const override;
        void execute() override;
    };

    //ADVANCE: move number of army units from source territory to target territory
    class Advance : public Order {

        //do we need a player attribute or a player container attribute for the player issuing the order?
        Territory* source;
        Territory* target;
        int numArmies;

        public:

        //function overrides
        bool validate() const override;
        void execute() override;
    };

    //BOMB: destroy half the army units on target territory, needs card 
    class Bomb : public Order {

        //do we need a player attribute or a player container attribute for the player issuing the order?
        Territory* target;
        //how to check the player's hand for a card?

        public:

        //function overrides
        bool validate() const override;
        void execute() override;
    };

    //BLOCKADE: triple number of army units on target territory and make it neutral, needs card
    class Blockade : public Order {

        //do we need a player attribute or a player container attribute for the player issuing the order?
        Territory* target;
        //how to check the player's hand for a card?
        //do we need an attribute to make it neutral?

        public:

        //function overrides
        bool validate() const override;
        void execute() override;
    };

    //AIRLIFT: advance number of army units from source territory to target territory, needs card
    class Airlift : public Order {

        //do we need a player attribute or a player container attribute for the player issuing the order?
        Territory* target;
        Territory* source;
        int numArmies;
        //how to check the player's hand for a card?

        public:

        //function overrides
        bool validate() const override;
        void execute() override;
    };

    //NEGOTIATE: prevent attacks between player and target player until the end of the turn, needs card
    class Negotiate : public Order {

        //do we need a player attribute or a player container attribute for the player issuing the order?
        //do we need a player attribute or a player container attribute for the target player?
        //how to check the player's hand for a card?

        public:

        //function overrides
        bool validate() const override;
        void execute() override;
    };

    /*------------------------------------------ORDERLIST CLASS--------------------------------------------------*/
    class OrderList {

    };

}
