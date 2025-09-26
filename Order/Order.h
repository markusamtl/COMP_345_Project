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
       
        public:

        //function overrides
        bool validate() const override;
        void execute() override;
    };

    //ADVANCE
    class Advance : public Order {

        public:

        //function overrides
        bool validate() const override;
        void execute() override;
    };

    //BOMN
    class Bomb : public Order {

        public:

        //function overrides
        bool validate() const override;
        void execute() override;
    };

    //BLOCKADE
    class Blockade : public Order {

        public:

        //function overrides
        bool validate() const override;
        void execute() override;
    };

    //AIRLIFT
    class Airlift : public Order {

        public:

        //function overrides
        bool validate() const override;
        void execute() override;
    };

    //NEGOTIATE
    class Negotiate : public Order {

        public:

        //function overrides
        bool validate() const override;
        void execute() override;
    };

    /*------------------------------------------ORDERLIST CLASS--------------------------------------------------*/
    class OrderList {

    };

    /*-------------------------------------------TEST-DRIVER FUNCTION---------------------------------------------*/

    void testOrdersList();

}