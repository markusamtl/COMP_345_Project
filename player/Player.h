#pragma once

#include "../map/map_storage/Territory.h"
#include "../card/Hand.h"
#include <iostream>
#include <string>
#include <map>
#include <vector>

using namespace std;

class player {

    private:

        string playerName;
        vector<Territory*> ownedTerritories;
        Hand* ownedCards; 

    public:


};