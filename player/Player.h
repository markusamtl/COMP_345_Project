#pragma once

#include "../map/map_storage/Territory.h"
#include "../card/Hand.h"
#include "../order/order_implementation/OrderList.h"
#include <iostream>
#include <string>
#include <map>
#include <vector>

using namespace std;

class player {

    private:

        string playerName;
        vector<Territory*> ownedTerritories;
        Hand* playerHand;
        OrderList* playerHand;

    public:

        //-- Class Methods -- 

        vector<Territory*> toAttack();
        vector<Territory*> toDefend();
        void issueOrders();


};