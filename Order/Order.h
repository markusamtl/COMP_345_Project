#pragma once

namespace WarzoneOrder {

    /*------------------------------------------ORDER SUPER-CLASS------------------------------------------------*/
    class Order {

    };

    /*------------------------------------------ SINGLE ORDER SUB-CLASSES------------------------------------------------*/

    class Deploy : public Order {

    };

    class Advance : public Order {

    };

    class Bomb : public Order {

    };

    class Blockade : public Order {

    };

    class Airlift : public Order {

    };

    class Negotiate : public Order {

    };

    /*------------------------------------------ORDERLIST CLASS--------------------------------------------------*/
    class OrderList {

    };

    /*-------------------------------------------TEST-DRIVER FUNCTION---------------------------------------------*/

    void testOrdersList();

}