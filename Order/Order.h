#pragma once

namespace WarzoneOrder {

    /*------------------------------------------ORDER SUPER-CLASS------------------------------------------------*/
    class Order {
        
        public:

        //pure virtual functions
        virtual bool validate() const =0;
        virtual void execute() =0;
    };

    /*------------------------------------------ SINGLE ORDER SUB-CLASSES------------------------------------------------*/

    //DEPLOY
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